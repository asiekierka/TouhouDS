#ifndef BOSS_SPELLS_H
#define BOSS_SPELLS_H

class BossNameSpell : public Spell {
    private:
        Param *nameP;

        char* name;

    public:
    	SPELL_STD(BossNameSpell)
        virtual void Reset();
};

class BossNumSpellcards : public Spell {
    private:
        Param *valueP;

        u8 value;

    public:
    	SPELL_STD(BossNumSpellcards)
        virtual void Reset();
};

class BossCallSpell : public Spell {
    private:
        Param *nameP, *lifeP, *timeP, *targetP;
        Param** params;
        int paramsL;

        const char* name;
        s32 life;
        u16 time;
        SpellBookTemplate* target;

    public:
        BossCallSpell(Param** p, SpellBookTemplate* target, Param** params, int paramsL);
        virtual ~BossCallSpell();

        virtual void Reset();
        SPELL_STD_METHODS(BossCallSpell)
};


#endif
