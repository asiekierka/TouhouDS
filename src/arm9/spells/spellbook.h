#ifndef SPELLBOOK_H
#define SPELLBOOK_H

#include "spell.h"

class SpellBook : public Spell {
    private:
        Spell** spells;
        int spellsL;
        int currentSpell;
        int totalLoops;
        int loopsCompleted;

    public:
        u32 wait;

        SpellBook(Spell** spells, int spellsL, int totalLoops=1);
        virtual ~SpellBook();

        virtual void Reset();
        virtual void Update();
        void Update(Game* game, Sprite* sprite);

        Spell** GetSpells();
        int GetSpellsL();
};

#endif
