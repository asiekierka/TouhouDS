#ifndef SPELL_H
#define SPELL_H

#include "../thcommon.h"

class Spell {
    private:

    protected:
		static Game* game;
		static SpellBook* spellBook;
		static Sprite* owner;

    public:
        bool finished;

        Spell() : finished(false) {
        };
        virtual ~Spell() {
        };

        virtual void Reset() {
        	finished = false;
        }
        virtual void Update() = 0;

};

#endif
