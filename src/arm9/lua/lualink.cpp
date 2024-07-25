#include "thlua.h"

//-----------------------------------------------------------------------------

LuaLink::LuaLink(lua_State* baseL)
:	wait(0), finished(false)
{
	L = lua_newthread(baseL);
	threadRef = luaL_ref(baseL, LUA_THREADREF_TABLE);
}

LuaLink::~LuaLink() {
	if (L && threadRef != LUA_REFNIL) {
		luaL_unref(L, LUA_THREADREF_TABLE, threadRef);
	}
}

bool LuaLink::AssertLUA(int errcode) {
    if (errcode == 0) {
    	finished = true;
    } else if (errcode != LUA_YIELD) {
        const char* str = lua_tostring(L, -1);
        log(EL_error, "LUA", "[err=%d] => %s", errcode, str);
		finished = true;
		return false;
    }
    return true;
}

int LuaLink::PushMethod(const char* methodName) {
	lua_getglobal(L, methodName);
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		return -1;
	}
	return 0;
}

bool LuaLink::Call(const char* methodName, const char* argTypes, ...) {
	int numargs = PushMethod(methodName);
	if (numargs < 0) {
		return false;
	}

	va_list vl;
	va_start(vl, argTypes);

	while (*argTypes != '\0') {
		switch (*argTypes) {
		case 'd': lua_pushinteger(L, va_arg(vl, int)); break;
		case 'f': lua_pushnumber(L, va_arg(vl, s32)); break;
		case 'b': lua_pushboolean(L, va_arg(vl, int) ? true : false); break;
		case 's': lua_pushstring(L, va_arg(vl, char*)); break;
		default: log(EL_warning, "LUA", "LuaLink.Call(), unknown argument type: %c", *argTypes);
		}
		numargs++;
		argTypes++;
	}

	va_end(vl);

	cur_luaLink = this;
	int errcode = lua_pcall(L, numargs, 0, 0);
	if (errcode != 0) {
		const char* str = lua_tostring(L, -1);
		log(EL_error, "LUA", "[err=%d] => %s", errcode, str);
		lua_pop(L, 1);
	}

	return errcode == 0;
}

void LuaLink::Update() {
	if (wait && --wait) {
		return;
	}

	cur_luaLink = this;
	if (!AssertLUA(lua_resume(L, 0))) {
		return;
	}

	if (finished && L && threadRef != LUA_REFNIL) {
		luaL_unref(L, LUA_THREADREF_TABLE, threadRef);
		threadRef = LUA_REFNIL;
	}
}

//-----------------------------------------------------------------------------

LuaFunctionLink::LuaFunctionLink(lua_State* baseL, const char* methodName)
:	LuaLink(baseL)
{
	lua_getglobal(L, methodName);
	if (lua_isnil(L, -1)) {
		finished = true;
	}
}

LuaFunctionLink::~LuaFunctionLink() {

}

//-----------------------------------------------------------------------------

LuaObjectLink::LuaObjectLink(lua_State* baseL, const char* luaClass)
:	LuaLink(baseL), inited(false)
{
	//Call static constructor (could be done at compile-time)
	lua_getglobal(L, luaClass);
	lua_getfield(L, -1, "new");

	cur_luaLink = this;
	if (!AssertLUA(lua_resume(L, 0))) {
		return;
	}
	objectRef = luaL_ref(L, LUA_OBJREF_TABLE);

	finished = false;
}
LuaObjectLink::LuaObjectLink(lua_State* baseL, int luaObjectRef)
:	LuaLink(baseL), inited(false)
{
	objectRef = luaObjectRef;
}

LuaObjectLink::~LuaObjectLink() {
	if (L && objectRef != LUA_REFNIL) {
		luaL_unref(L, LUA_OBJREF_TABLE, objectRef);
	}
}

int LuaObjectLink::PushMethod(const char* methodName) {
	//int oldtop = lua_gettop(L);

	lua_rawgeti(L, LUA_OBJREF_TABLE, objectRef);
	lua_getfield(L, -1, methodName);

	//printf("%d %d (%d) %s\n", oldtop, lua_gettop(L), objectRef, methodName);

	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return -1;
	}

	lua_pushvalue(L, -2);
	lua_remove(L, -3);

	return 1;
}

bool LuaObjectLink::Init() {
	//Call init
	lua_rawgeti(L, LUA_OBJREF_TABLE, objectRef);
	lua_getfield(L, -1, "init");
	if (lua_isfunction(L, -1)) {
		lua_rawgeti(L, LUA_OBJREF_TABLE, objectRef);
		if (!AssertLUA(lua_resume(L, 1))) {
			log(EL_error, "LUA.init", "ref=%d\n", objectRef);
			return false;
		}
	} else {
		lua_pop(L, 1);
	}

	//AssertLUA sets finished to true once a method completes, reset for call to update
	finished = false;

	//Do the first call to the update function (first call requires 'self' arg)
	lua_getfield(L, -1, "update");
	if (lua_isfunction(L, -1)) {
		lua_rawgeti(L, LUA_OBJREF_TABLE, objectRef);
		if (!AssertLUA(lua_resume(L, 1))) {
			log(EL_error, "LUA.update", "ref=%d\n", objectRef);
			return false;
		}
		return true;
	} else {
		lua_pop(L, 1);
		finished = true;
		return false;
	}
}

void LuaObjectLink::Update() {
	if (wait && --wait) {
		return;
	}

	cur_luaLink = this;

	if (!inited) {
		Init();
		inited = true;
	} else {
		if (!AssertLUA(lua_resume(L, 0))) {
			return;
		}
	}
}

//-----------------------------------------------------------------------------

LuaMethodLink::LuaMethodLink(lua_State* baseL, int objectRef, const char* mname)
:	LuaObjectLink(baseL, objectRef), looping(false)
{
	methodName = strdup(mname);
}
LuaMethodLink::~LuaMethodLink() {
	free(methodName);

	objectRef = LUA_REFNIL; //Avoid removal of object, we're not the owner
}

bool LuaMethodLink::Init() {
	//Call method
	lua_rawgeti(L, LUA_OBJREF_TABLE, objectRef);
	lua_getfield(L, -1, methodName);
	if (lua_isfunction(L, -1)) {
		lua_rawgeti(L, LUA_OBJREF_TABLE, objectRef);
		int errcode = lua_resume(L, 1);

		if (errcode == 0) {
			finished = true;
		} else if (errcode != LUA_YIELD) {
			finished = true;
			log(EL_warning, "LUA", "%s\n", lua_tostring(L, -1));
			return false;
		}

		return true;
	} else {
		lua_pop(L, 2);

		finished = true;
		return false;
	}
}

void LuaMethodLink::Update() {
	if (wait && --wait) {
		return;
	}

	cur_luaLink = this;

	if (!inited) {
		if (Init()) {
			inited = true;
		} else {
			looping = false;
			finished = true;
		}
	} else {
		if (!AssertLUA(lua_resume(L, 0))) {
			looping = false;
			finished = true;
		}
	}

	if (finished) {
		if (looping) {
			inited = finished = false;
		} else if (L && threadRef != LUA_REFNIL) {
			luaL_unref(L, LUA_THREADREF_TABLE, threadRef);
			threadRef = LUA_REFNIL;
		}
	}
}
