#ifndef LUALINK_H
#define LUALINK_H

#include "thlua.h"

class LuaLink {
	protected:
		lua_State* L;
		int threadRef;

		bool AssertLUA(int errcode);
		virtual int PushMethod(const char* methodName);

	public:
		u32 wait;
		bool finished;

		LuaLink(lua_State* L);
		virtual ~LuaLink();

		virtual bool Call(const char* methodName, const char* argTypes, ...);
		virtual void Update();
};

class LuaFunctionLink : public LuaLink {
	private:

	public:
		LuaFunctionLink(lua_State* L, const char* methodName);
		virtual ~LuaFunctionLink();
};

class LuaObjectLink : public LuaLink {
	protected:
		bool inited;

		virtual bool Init();
		virtual int PushMethod(const char* methodName);

	public:
		int objectRef;

		LuaObjectLink(lua_State* L, const char* luaClass);
		LuaObjectLink(lua_State* L, int luaObjectRef);
		virtual ~LuaObjectLink();

		virtual void Update();

};

class LuaMethodLink : public LuaObjectLink {
	protected:
		char* methodName;

		virtual bool Init();

	public:
		bool looping;

		LuaMethodLink(lua_State* L, int objectRef, const char* methodName);
		virtual ~LuaMethodLink();

		virtual void Update();
};

#endif
