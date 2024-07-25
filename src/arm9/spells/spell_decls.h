#ifndef SPELL_DECLS_H
#define SPELL_DECLS_H

#include "../thcommon.h"
#include "../parser/parser_c.h"

enum SpellParamType {
	SPT_null = 0,
//Primitives
	SPT_int = 1,
	SPT_intConst = 2,
	SPT_float = 4,
	SPT_floatConst = 8,
	SPT_string = 16,
	SPT_stringConst = 32,
//Objects
	SPT_spell = 64,
	SPT_object = 128,
	SPT_boss = 256,
	SPT_conversation = 512,
	SPT_animation = 1024,
//Enums
	SPT_texture = 2048,
	SPT_itemType = 4096,
	SPT_angleTarget = 8192,
	SPT_charaAnimationType = 16384,
	SPT_drawAngleMode = 32768,

//Special

	//Signifies the start of a call: call, call_object_type, params...
	SPT_callFlag = 65536
};

class EnumType {
	private:
		char* optionString;
		char** options;
		int optionsL;

	public:
		EnumType(const char* options);
		virtual ~EnumType();

		bool IsMember(const char* str);
};

class SpellDecl {
	private:
		char* name;
		u32 validContexts;
		SpellParamType typeSignatures[8][8];
		int typeSignaturesL;
		Spell* (*createFunc) (Game*, u16, Param**, u8);

		int TypeCheckParam(Runtime* runtime, SpellBookTemplate* sbt, SpellTemplate* spell,
				int sig, int* typeParamIndex, int paramIndex, bool printErrors);
		bool TypeCheckCall(Runtime* runtime, int fileId, SpellTemplate* spell,
				int sig, int paramIndex);

	public:
		SpellDecl(const char* name, Spell* (*createFunc)(Game*, u16, Param**, u8));
		virtual ~SpellDecl();

		bool TypeCheck(Runtime* runtime, SpellBookTemplate* sbt, SpellTemplate* spell);
		Spell* ToSpell(Game* game, u16 fileId, Param** globals, u8 globalsL);

		const char* GetName();
		bool IsValidContext(u32 c);

		void SetTypesig(int index, ...);
		void SetValidContexts(u32 c);

};

extern SpellDecl* sd_loop;
extern SpellDecl* sd_for;

void initSpellDecls();
SpellDecl* getSpellDecl(const char* name);

#endif
