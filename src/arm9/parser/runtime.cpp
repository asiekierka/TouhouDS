#include "runtime.h"

#include <ctype.h>
#include <vector>
#include <string>
#include "parser.h"
#include "compiler.h"
#include "../thlog.h"
#include "../lua/thlua.h"
#include "../tcommon/parser/ini_parser.h"

using namespace std;

//------------------------------------------------------------------------------

Runtime::Runtime() {
    L = NULL;
}
Runtime::~Runtime() {
    Reset();
}

void Runtime::Reset() {
	for (u32 n = 0; n < spellBookTemplates.size(); n++) {
		for (u32 x = 0; x < spellBookTemplates[n].size(); x++) {
            if (spellBookTemplates[n][x]) delete spellBookTemplates[n][x];
		}
	}
	spellBookTemplates.clear();
	fileIdNames.clear();

	if (L) {
		lua_close(L);
		L = NULL;
	}
}

int Runtime::Compile(const char* levelScriptFolder, const char* charaScriptFolder) {
	vector<const char*> scriptFolders;
	scriptFolders.push_back(levelScriptFolder);
	scriptFolders.push_back(charaScriptFolder);

	return Compile(scriptFolders);
}

int Runtime::Compile(vector<const char*>& scriptFolders) {
	int result = 0;

	fileIdNames.clear();
	spellBookTemplates.clear();

	//Collect files
	vector<string> scriptFiles;
	for (u32 n = 0; n < scriptFolders.size(); n++) {
	    FileList* fileList = new FileList(scriptFolders[n], ".pds");

	    const char* filename;
	    while ((filename = fileList->NextFile()) != NULL) {
	    	char path[MAXPATHLEN];
	        sprintf(path, "%s/%s", scriptFolders[n], filename);
	        scriptFiles.push_back(string(path));

	        //File ID's are the filenames without file-ext and in lower-case
            const char* fn = strrchr(filename, '/');
            if (fn) {
            	fn++;
            } else {
            	fn = filename;
            }

            int t = 0;
            while (fn[t] != '\0' && fn[t] != '.') {
            	path[t] = tolower(fn[t]);
            	t++;
            }
            path[t++] = '\0';
	        fileIdNames.push_back(string(path));
	    }
    	delete fileList;
	}

    //Parse & Compile
    for (u32 n = 0; result == 0 && n < scriptFiles.size(); n++) {
    	result = loadScript(scriptFiles[n].c_str());
        if (result == 0) {
			spellBookTemplates.push_back(vector<SpellBookTemplate*>());

			ParseNode* node = defines;
			while (node) {
				ParseNode* define = node->defineList.value;
				SpellBookTemplate* sbt = (define ? compileDefine(this, n, define) : NULL);
				spellBookTemplates[n].push_back(sbt);
				node = node->defineList.next;
			}
        }
    }

    //Type checking
    for (u32 n = 0; result == 0 && n < spellBookTemplates.size(); n++) {
    	for (u32 x = 0; x < spellBookTemplates[n].size(); x++) {
            if (!checkSpellContext(this, spellBookTemplates[n][x])
            		|| !checkSpellType(this, spellBookTemplates[n][x]))
            {
                result = COMPILE_TYPE_ERROR;
            }
    	}
    }

    return result;
}

int Runtime::CompileLuaBase() {
	if (L) lua_close(L);

	//Init LUA state
	L = lua_open();

	//Open base libraries
	static const luaL_reg lualibs[] = {
	    {"base", luaopen_base},
	    {"table", luaopen_table},
	    //{"io", luaopen_io},
	    {"string", luaopen_string},
	    //{"math", luaopen_math},
    	{NULL, NULL}
	};

	for (const luaL_reg* lib = lualibs; lib->func != NULL; lib++) {
	    lua_pushcfunction(L, lib->func);
	    lua_pushstring(L, lib->name);
	    lua_call(L, 1, 0);
	}

    luaRegisterFunctions(L);

    return 0;
}

int Runtime::CompileLuaFolder(const char* scriptFolder) {
	//Compile lua code
	FileList* fileList = new FileList(scriptFolder, ".lua");
    if (fileList->GetFilesL() == 0) {
    	delete fileList;
        return 0;
    }

    bool compileLUA = false;
	vector<char> compiled;

	char* filename;
    while ((filename = fileList->NextFile()) != NULL) {
        char code[512];
        int err;

        if (!compileLUA) {
        	sprintf(code, "%s/%s", scriptFolder, filename);
        	err = luaL_loadfile(L, code);
            if (err != 0) {
            	luaCompileError(L, err);
    			return COMPILE_SYNTAX_ERROR;
            }

            err = lua_pcall(L, 0, 0, 0);
            if (err != 0) {
            	luaCompileError(L, err);
    			return COMPILE_SYNTAX_ERROR;
            }
        	continue;
        }

    	sprintf(code, "__compiled = string.dump(loadfile(\"%s/%s\"))", scriptFolder, filename);
        err = luaL_dostring(L, code);
        if (err != 0) {
        	luaCompileError(L, err);
			return COMPILE_SYNTAX_ERROR;
        }

        lua_getglobal(L, "__compiled");
        const char* bytes = lua_tostring(L, -1);
        size_t bytesL = lua_strlen(L, -1);

        while (bytesL > 0) {
        	compiled.push_back(*bytes);
        	bytes++; bytesL--;
        }

        lua_pushnil(L);
        lua_setglobal(L, "__compiled");

        //lua_newtable(L);
        //lua_replace(L, LUA_GLOBALSINDEX);
    }

    if (compileLUA) {
    	int err = 0;

		//Put compiled code into Lua engine
		err = luaL_loadbuffer(L, &compiled[0], compiled.size(), "_user_chunk");
		if (err) {
			luaCompileError(L, err);
			return COMPILE_SYNTAX_ERROR;
		}
		err = lua_pcall(L, 0, 0, 0);
		if (err) {
			luaCompileError(L, err);
			return COMPILE_SYNTAX_ERROR;
		}
    }

    delete fileList;
    return 0;
}

ITCM_CODE
int Runtime::GetFileId(const char* fileName) {
    for (u32 n = 0; n < fileIdNames.size(); n++) {
        if (strcmp(fileIdNames[n].c_str(), fileName) == 0) {
            return n;
        }
    }
    return -1;
}

ITCM_CODE
SpellBookTemplate* Runtime::GetSpellTemplate(int fileId, const char* name, bool suppressErrors) {
    char* temp = strchr(name, '.');
    if (temp) { //name is in (file.id) form, we need to find the correct fileId
        temp[0] = '\0'; //cutoff (.id) part
        fileId = GetFileId(name);
        temp[0] = '.'; //reattach (.id) part
    }

    if (fileId < 0) {
        return NULL;
    }

    const char* needle = (temp ? temp+1 : name);
    vector<SpellBookTemplate*> sbt = spellBookTemplates[fileId];
    for (u32 n = 0; n < sbt.size(); n++) {
        if (strcmp(sbt[n]->name, needle) == 0) {
            return sbt[n];
        }
    }

    if (!suppressErrors) {
		log(EL_error, __FILENAME__, "Spell not found: fileId=%d name=%s", fileId, name);
		for (int n = 0; n < 100; n++) swiWaitForVBlank();
    }
    return NULL;
}
