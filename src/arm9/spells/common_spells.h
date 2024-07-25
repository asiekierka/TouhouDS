#ifndef COMMON_SPELLS_H
#define COMMON_SPELLS_H

#include "spell_decls.h"
#include "spellbook.h"

#define SPELL_STD_CONSTR(x) \
	x(Game* game, Param** p, int pL); \
	virtual ~x(); \

#define SPELL_STD_METHODS(x) \
	virtual void Update(); \
	static void CreateDecls(SpellDecl** decls, int& declsL); \
	static Spell* ToSpell(Game* game, u16 fileId, Param** p, u8 pL); \

#define SPELL_STD(x) \
	SPELL_STD_CONSTR(x) \
	SPELL_STD_METHODS(x) \

#define TO_SPELL(x) \
	Spell* x::ToSpell(Game* game, u16 fileId, Param** p, u8 pL) { \
		return new x(game, p, pL); \
	} \

#define MAX_SPELL_PARAMS 6

#include "level_spells.h"
#include "conv_spells.h"
#include "object_spells.h"
#include "boss_spells.h"
#include "create_spells.h"
#include "lua_spells.h"

#endif
