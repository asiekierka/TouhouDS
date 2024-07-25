#ifndef COMPILER_H
#define COMPILER_H

#include "../thcommon.h"
#include "thds.tab.hh"
#include "parser_c.h"

class Param {
    private:

    public:
        ParamType type;
        u16 refcount;

        union {
        	struct {
                s32 numval; //Floats are also stored in this attribute (in 20.12 format)
                char* strval;
                SpellBookTemplate* cachedSpell;
        	} normal;
        	struct {
                s32 min;
                s32 max;
                s32 step;
        	} random;
        };

        Param();
        ~Param();

        int AsInt();
        int AsFixed();
        char* AsString();

        const Texture* AsTexture(Game* game);
        ItemType AsItemType();
        SpellBookTemplate* AsSpell(Game* game, u16 fileId);
        AngleTarget AsAngleTarget();
        CharaAnimationType AsCharaAnimationType();
        DrawAngleMode AsDrawAngleMode();

        void ToString(char* out);

        void SetString(const char* str);
};

class SpellTemplate {
    private:

    public:
        SpellDecl* decl;
        char* name;
        Param** params;
        int paramsL;

        SpellTemplate();
        virtual ~SpellTemplate();

        void SetName(const char* name);
        void AddParam(Param* param);
        Spell* ToSpell(Game* game, u16 fileId, Param** globals = NULL, u8 globalsL = 0);

        virtual void ToString(char* out);
};

class SpellBookTemplate {
    private:

    public:
        int type;
        u16 fileId;
        u16 spellsL;
        char* name;
        SpellTemplate** spells;

        SpellBookTemplate(u16 fileId, int type = SPELL);
        virtual ~SpellBookTemplate();

        void SetName(char* s);
        void AddSpell(SpellTemplate* spell);

        SpellBook* ToSpellBook(Game* game, SpellTemplate** spells,
            int spellsL, Param** params = NULL, u8 paramsL = 0);

        u8 GetNumberOfVariables();
};

class ObjectTemplate : public SpellBookTemplate {
    private:
        void InitObject(Game* game, Sprite* sprite, Param** params, u8 paramsL);

    public:
        u16 initSpellsStart;
        u16 lifeSpellsStart;
        u16 deathSpellsStart;

        ObjectTemplate(u16 fileId, int type = OBJECT);
        virtual ~ObjectTemplate();

        Sprite* ToObject(Game* game, SpriteType spriteType, Param** params = NULL, u8 paramsL = 0);
        Boss* ToBoss(Game* game, Param** params = NULL, u8 paramsL = 0);
};

SpellBookTemplate* compileDefine(Runtime* runtime, u16 fileId, ParseNode* node);
SpellTemplate* compileSpell(Runtime* runtime, ParseNode* node);
bool compileParam(Param* retval, Runtime* runtime, ParseNode* node);

bool checkSpellContext(Runtime* runtime, SpellBookTemplate* sbt);
bool checkSpellType(Runtime* runtime, SpellBookTemplate* sbt);

inline
Param* resolveVar(Param* p, Param** globals, u8 globalsL) {
	return p->type == PT_variable && p->normal.numval >= 1 && p->normal.numval <= globalsL
		? globals[p->normal.numval-1]
		: p;
}

inline
Param* refParam(Param* p) {
#ifdef DEBUG
	if (!p) log(EL_error, "?", "Referencing NULL param.");
#endif

	p->refcount++;
	return p;
}

inline
void unrefParam(Param* p) {
#ifdef DEBUG
	if (!p) log(EL_error, "?", "Referencing NULL param.");
#endif

	p->refcount--;
	if (!p->refcount) {
		delete p;
	}
}

//-----------------------------------------------------------------------------

#endif
