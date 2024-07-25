#ifndef LEVEL_SPELLS_H
#define LEVEL_SPELLS_H

#include "level_spells.h"

class WaitSpell : public Spell {
    private:
        Param *amountP;

        u32 amount;

    public:
    	SPELL_STD(WaitSpell)
        virtual void Reset();
};

class EndStageSpell : public Spell {
    private:
        u32 delay;

    public:
    	SPELL_STD(EndStageSpell)
        virtual void Reset();
};

class BGMSpell : public Spell {
    private:
        Param *bgmP;

        const char* bgm;
        u8 frame;

    public:
    	SPELL_STD(BGMSpell)
        virtual void Reset();
};

class SFXSpell : public Spell {
    private:
        Param *sfxP;

        const char* sfx;

    public:
    	SPELL_STD(SFXSpell)
        virtual void Reset();
};

class BGControlSpell : public Spell {
    private:
        Param *xP, *yP;

        s32 x, y;

    public:
    	SPELL_STD(BGControlSpell)
        virtual void Reset();
};

#endif
