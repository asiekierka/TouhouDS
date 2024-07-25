#include "spellbook.h"
#include "../lua/thlua.h"

SpellBook::SpellBook(Spell** s, int sL, int loops)
:	Spell(), spellsL(sL), currentSpell(0), totalLoops(loops), loopsCompleted(0),
 	wait(0)
{
	spells = new Spell*[sL];
	for (int n = 0; n < sL; n++) {
		spells[n] = s[n];
	}

    if (spellsL == 0) {
        totalLoops = 0;
        finished = true;
    }
}
SpellBook::~SpellBook() {
	for (int n = 0; n < spellsL; n++) {
		if (spells[n]) delete spells[n];
	}
	delete[] spells;
}

ITCM_CODE
void SpellBook::Reset() {
    Spell::Reset();

    for (int n = 0; n < spellsL; n++) {
        if (spells[n]) spells[n]->Reset();
    }
    currentSpell = 0;
    loopsCompleted = 0;
    wait = 0;
}

ITCM_CODE
void SpellBook::Update(Game* g, Sprite* s) {
	SpellBook* oldSpellBook = spellBook;

	game = cur_game = g;
	spellBook = this;
	owner = cur_sprite = s;

	Update();

	spellBook = oldSpellBook;
}

ITCM_CODE
void SpellBook::Update() {
	if (wait) {
		--wait;
		return;
	}

	SpellBook* oldSpellBook = spellBook;
	spellBook = this;

	Spell* cur = NULL;
	while (!wait && currentSpell < spellsL) {
		cur = spells[currentSpell];
		if (cur && !cur->finished) {
			cur->Update();
			if (!cur->finished) {
				spellBook = oldSpellBook;
				return;
			}
		}

		currentSpell++;
    }

	if (!wait && currentSpell >= spellsL) {
		loopsCompleted++;
		if (loopsCompleted >= totalLoops) {
			finished = true;
			if (totalLoops <= 0) {
				Reset();
			}
		} else if (loopsCompleted < totalLoops) {
			currentSpell = 0;
			for (int n = 0; n < spellsL; n++) {
				if (spells[n]) spells[n]->Reset();
			}
		}
	}

	spellBook = oldSpellBook;
}

Spell** SpellBook::GetSpells() {
    return spells;
}
int SpellBook::GetSpellsL() {
    return spellsL;
}
