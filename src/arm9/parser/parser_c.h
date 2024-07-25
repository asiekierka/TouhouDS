#ifndef PARSER_C_H
#define PARSER_C_H

#include <stdlib.h>
#include <string.h>

/*
Defines the external C interface for the parser. It's just to let lex read from
a buffer instead of stdin.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PN_defineList,
    PN_define,
    PN_paramList,
    PN_param,
    PN_spell,
    PN_spellBook,
    PN_label
} ParseNodeType;

typedef enum {
	PT_null,
    PT_variable,
    PT_int,
    PT_float,
    PT_string,
    PT_identifier,
	PT_random
} ParamType;

struct ParseNode {
    ParseNodeType type;

    union {
        struct {
            struct ParseNode* value;
            struct ParseNode* next;
        } defineList;
        struct {
            int type;
            char name[64];
            struct ParseNode* spellBook;
        } define;
        struct {
            struct ParseNode* value;
            struct ParseNode* next;
        } paramList;
        struct {
            ParamType type;
            int intval;
            float floatval;
            char strval[256];
        } param;
        struct {
            char name[256];
        } label;
        struct {
            char name[256];
            struct ParseNode* params;
        } spell;
        struct {
            struct ParseNode* value;
            struct ParseNode* next;
        } spellBook;
    };
};


struct ParseNode* defineList(struct ParseNode* defineList, struct ParseNode* define);
void freeDefineList(struct ParseNode* node);
struct ParseNode* define(int type, char* name, struct ParseNode* spellBook);
void freeDefine(struct ParseNode* node);
struct ParseNode* spellBook(struct ParseNode* spellBook, struct ParseNode* spell);
void freeSpellBook(struct ParseNode* node);
struct ParseNode* paramList(struct ParseNode* paramList, struct ParseNode* param);
void freeParamList(struct ParseNode* node);
struct ParseNode* spell(char* name, struct ParseNode* params);
void freeSpell(struct ParseNode* node);
struct ParseNode* label(char* name);
void freeLabel(struct ParseNode* node);
struct ParseNode* param(ParamType type);
struct ParseNode* parami(ParamType type, int intval);
struct ParseNode* paramf(ParamType type, float floatval);
struct ParseNode* params(ParamType type, char* strval);
struct ParseNode* paramo(ParamType type, struct ParseNode* node);
void freeParam(struct ParseNode* node);






int scriptFromMem(const char* data, int dataL);
int scriptGetInput(char* data, int dataL);
int yylex();
void yyerror(const char* s);

extern int yyparse();
extern void clearStringPool();
extern int parserLine;
extern struct ParseNode* defines;
extern char* parseErrors;

#ifndef NULL
#define NULL 0
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifdef __cplusplus
}
#endif

#endif
