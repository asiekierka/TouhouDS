#ifndef CREATE_SPELLS_H
#define CREATE_SPELLS_H

#include "../parser/compiler.h"

class AnimationSpell : public Spell {
    private:
        SpellBookTemplate* target;
        Param* params[MAX_SPELL_PARAMS];
        int paramsL;

    public:
    	AnimationSpell(SpellBookTemplate* target, Param** params, int paramsL);
        virtual ~AnimationSpell();

        SPELL_STD_METHODS(AnimationSpell)
};

class CharaAnimationSpell : public Spell {
    private:
    	Param* animTypeP;
        SpellBookTemplate* target;
        Param* params[MAX_SPELL_PARAMS];
        int paramsL;

        CharaAnimationType animType;

    public:
    	CharaAnimationSpell(Param* animType, SpellBookTemplate* target, Param** params, int paramsL);
        virtual ~CharaAnimationSpell();

        virtual void Reset();
        SPELL_STD_METHODS(CharaAnimationSpell)
};

class FireSpell : public Spell {
    private:
        ObjectTemplate* projectile;
        Param* params[MAX_SPELL_PARAMS];
        int paramsL;

    public:
        FireSpell(ObjectTemplate* projectile, Param** params, int paramsL);
        virtual ~FireSpell();

        SPELL_STD_METHODS(FireSpell)
};

class CreateSpell : public Spell {
    private:
        ObjectTemplate* object;
        Param* params[MAX_SPELL_PARAMS];
        int paramsL;
        bool createAsBoss;

    public:
        CreateSpell(ObjectTemplate* object, Param** params, int paramsL, bool createAsBoss=false);
        virtual ~CreateSpell();

    	virtual void Update();
    	static void CreateDecls(SpellDecl** decls, int& declsL);
};

class CallSpell : public Spell {
    private:
        SpellBookTemplate* target;
        Param* params[MAX_SPELL_PARAMS];
        int paramsL;

        SpellBook* spellbook;

    public:
        CallSpell(SpellBookTemplate* target, Param** params, int paramsL);
        virtual ~CallSpell();

        virtual void Reset();
        SPELL_STD_METHODS(CallSpell)
};

class ForkSpell : public Spell {
    private:
        SpellBookTemplate* target;
        Param* params[MAX_SPELL_PARAMS];
        int paramsL;

    public:
        ForkSpell(SpellBookTemplate* target, Param** params, int paramsL);
        virtual ~ForkSpell();

        SPELL_STD_METHODS(ForkSpell)
};

class ConversationSpell : public Spell {
    private:
        SpellBookTemplate* target;
        Param* params[MAX_SPELL_PARAMS];
        int paramsL;

    public:
    	ConversationSpell(SpellBookTemplate* target, Param** params, int paramsL);
        virtual ~ConversationSpell();

        SPELL_STD_METHODS(ConversationSpell)
};

class ForSpell : public Spell {
    private:
    	Game* game;
    	Param *startP, *endP, *stepP;
    	int fileId;
        SpellTemplate* target;
        Param* globals[MAX_SPELL_PARAMS];
        int globalsL;

        s32 start, end, step;
        Spell* spell;

    public:
    	ForSpell(Game* g, int fileId, Param** p, int pL, SpellTemplate* target, Param** globals, int globalsL);
        virtual ~ForSpell();

        virtual void Reset();
    	virtual void Update();
};

#endif
