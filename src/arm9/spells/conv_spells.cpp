#include "common_spells.h"
#include "../conversation.h"
#include "../game.h"
#include "../player.h"
#include "../parser/compiler.h"

//==============================================================================

SaySpell::SaySpell(Game* g, Param** p, int pL) {
	slotP = refParam(p[0]);
	nameP = refParam(p[1]);
	textP = refParam(p[2]);

	Reset();
}
SaySpell::~SaySpell() {
	unrefParam(slotP);
	unrefParam(nameP);
	unrefParam(textP);
}

void SaySpell::Reset() {
	slot = slotP->AsInt();
	name = nameP->AsString();
	text = textP->AsString();
	waitingForInput = false;

	Spell::Reset();
}
void SaySpell::Update() {
	if (!waitingForInput) {
		Conversation* conv = game->GetConversation();
		if (conv) {
			conv->Say(slot, name, text);
		}
		waitingForInput = true;
	} else {
        if (keysDown() & (KEY_TOUCH|KEY_A|KEY_B|KEY_X|KEY_Y|KEY_START)) {
    		finished = true;
        }
	}
}

void SaySpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* decl = new SpellDecl("say", &SaySpell::ToSpell);
	decl->SetValidContexts(SPT_conversation);
	decl->SetTypesig(0, SPT_int, SPT_string, SPT_string, SPT_null);
	decls[declsL++] = decl;
}
TO_SPELL(SaySpell)

//==============================================================================

ConvImageSpell::ConvImageSpell(Game* g, Param** p, int pL) {
	slotP = refParam(p[0]);
	imageP = (pL > 1 ? refParam(p[1]) : NULL);

	Reset();
}
ConvImageSpell::~ConvImageSpell() {
	unrefParam(slotP);
	if (imageP) unrefParam(imageP);
}

void ConvImageSpell::Reset() {
	slot = slotP->AsInt();
	image = (imageP ? imageP->AsString() : NULL);

	Spell::Reset();
}
void ConvImageSpell::Update() {
	Conversation* conv = game->GetConversation();
	if (conv) {
		conv->Image(slot, image);
	}
	finished = true;
}


Spell* create_convClear(Game* game, u16 fileId, Param** p, u8 pL) {
	return new ConvImageSpell(game, p, 1);
}
Spell* create_convImage(Game* game, u16 fileId, Param** p, u8 pL) {
	return new ConvImageSpell(game, p, pL);
}

void ConvImageSpell::CreateDecls(SpellDecl** decls, int& declsL) {
	SpellDecl* clearDecl = new SpellDecl("conv_clear", &create_convClear);
	clearDecl->SetValidContexts(SPT_conversation);
	clearDecl->SetTypesig(0, SPT_int, SPT_null);
	decls[declsL++] = clearDecl;
	SpellDecl* imageDecl = new SpellDecl("conv_image", &create_convImage);
	imageDecl->SetValidContexts(SPT_conversation);
	imageDecl->SetTypesig(0, SPT_int, SPT_string, SPT_null);
	decls[declsL++] = imageDecl;
}

//==============================================================================
