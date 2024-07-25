#include "common_spells.h"
#include "../game.h"
#include "../sprite.h"
#include "../lua/thlua.h"

//=============================================================================

LuaSpell::LuaSpell(Game* game, Param** ps, int psL) {
    paramsL = MIN(MAX_SPELL_PARAMS, psL);
    for (int n = 0; n < paramsL; n++) {
        params[n] = refParam(ps[n]);
    }

    luaLink = NULL;
}

LuaSpell::~LuaSpell() {
	for (int n = 0; n < paramsL; n++) {
		unrefParam(params[n]);
	}
	if (luaLink) delete luaLink;
}

void LuaSpell::Reset() {
	Spell::Reset();

	if (luaLink) delete luaLink;

	if (paramsL > 0) {
		luaLink = new LuaLink(cur_game->runtime.L);
		luaLink->Call(params[0]->AsString(), "");
	}
}

void LuaSpell::Update() {
	if (luaLink && !luaLink->finished) {
		luaLink->Update();
	}
	if (!luaLink || luaLink->finished) {
		finished = true;
	}
}

void LuaSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("lua", &LuaSpell::ToSpell);
	decl->SetTypesig(0, SPT_string, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(LuaSpell)

//=============================================================================
