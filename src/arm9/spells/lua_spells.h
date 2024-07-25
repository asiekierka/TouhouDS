#ifndef LUA_SPELLS_H
#define LUA_SPELLS_H

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

class LuaSpell : public Spell {
    private:
        Param* params[MAX_SPELL_PARAMS];
        int paramsL;

        LuaLink* luaLink;

    public:
    	SPELL_STD(LuaSpell)

    	virtual void Reset();
};

#endif
