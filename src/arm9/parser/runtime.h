#ifndef RUNTIME_H
#define RUNTIME_H

#include <nds.h>
#include <string>
#include <vector>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

class SpellBookTemplate;

#define COMPILE_FILE_NOT_FOUND  1001
#define COMPILE_DIR_NOT_FOUND   1002
#define COMPILE_TYPE_ERROR      1003
#define COMPILE_MISSING_MAIN    1004
#define COMPILE_SYNTAX_ERROR	1005

class Runtime {
    private:
        std::vector<std::string> fileIdNames;
        std::vector<std::vector<SpellBookTemplate*> > spellBookTemplates;

    public:
        lua_State* L; //LUA interpreter

        Runtime();
        virtual ~Runtime();

        void Reset();
        int Compile(const char* levelScriptFolder, const char* charaScriptFolder);
        int Compile(std::vector<const char*>& scriptFolders);
        int CompileLuaBase();
        int CompileLuaFolder(const char* scriptFolder);

        SpellBookTemplate* GetSpellTemplate(int fileId, const char* name, bool suppressErrors=false);
        int GetFileId(const char* fileName);
};

#endif
