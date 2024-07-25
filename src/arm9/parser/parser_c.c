#include "parser_c.h"
#include <stdio.h>

#define PARSE_ERRORS_LENGTH (16*1024)

int parserLine = 0;
struct ParseNode* defines = NULL;
char* parseErrors = NULL;
int parseNodes = 0;

static struct {
	const char *data;
	int dataL;
} scriptReadStruct;

int scriptFromMem(const char* data, int dataL) {
	scriptReadStruct.data = data;
	scriptReadStruct.dataL = dataL;

    //Allocate parser errors memory
    if (parseErrors) {
        free(parseErrors);
    }
    parseErrors = (char*)malloc(PARSE_ERRORS_LENGTH);

    //Reset parser vars
    clearStringPool();
    parseErrors[0] = '\0';
    parserLine = 1;
    if (defines) {
        freeDefineList(defines);
        defines = NULL;
    }

    //Parse
	return yyparse();
}

int scriptGetInput(char* data, int dataL) {
    if (dataL > scriptReadStruct.dataL) {
        dataL = scriptReadStruct.dataL;
    }

    memcpy(data, scriptReadStruct.data, dataL);
    scriptReadStruct.data += dataL;
    scriptReadStruct.dataL -= dataL;

    return dataL;
}

void yyerror(const char* s) {
    char temp[1024];
    sprintf(temp, "%d: %s\n", parserLine, s);

    int parseErrorsL = strlen(parseErrors);
    if (parseErrorsL + strlen(temp) < PARSE_ERRORS_LENGTH-1) {
    	strcat(parseErrors+parseErrorsL, temp);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



struct ParseNode* defineList(struct ParseNode* defineList, struct ParseNode* define) {
    //strcat(parseErrors, "dl ");
    parseNodes++;

    struct ParseNode* next = (struct ParseNode*)malloc(sizeof(struct ParseNode));
    next->type = PN_defineList;
    if (defineList == NULL) {
        defineList = next;
    } else {
        //Add value to the end of the list
        struct ParseNode* node = defineList;
        while (node->defineList.next) {
            node = node->defineList.next;
        }
        node->defineList.next = next;
    }

    next->defineList.value = define;
    next->defineList.next = NULL;

    return defineList;
}
void freeDefineList(struct ParseNode* node) {
    if (!node) return;

    struct ParseNode* next;
    while (node) {
        next = node->defineList.next;
        freeDefine(node->defineList.value);
        free(node); parseNodes--;
        node = next;
    }
}

struct ParseNode* define(int type, char* name, struct ParseNode* spellBook) {
    //strcat(parseErrors, "d ");
    parseNodes++;

    struct ParseNode* result = (struct ParseNode*)malloc(sizeof(struct ParseNode));
    result->type = PN_define;

    result->define.type = type;

    strncpy(result->define.name, name, 63);
    result->define.name[63] = '\0';

    result->define.spellBook = spellBook;

    return result;
}
void freeDefine(struct ParseNode* node) {
    if (!node) return;

    freeSpellBook(node->define.spellBook);
    free(node); parseNodes--;
}

struct ParseNode* spellBook(struct ParseNode* spellBook, struct ParseNode* spell) {
    //strcat(parseErrors, "sb ");
    parseNodes++;

    struct ParseNode* next = (struct ParseNode*)malloc(sizeof(struct ParseNode));
    next->type = PN_spellBook;
    if (spellBook == NULL) {
        spellBook = next;
    } else {
        //Add value to the end of the list
        struct ParseNode* node = spellBook;
        while (node->spellBook.next) {
            node = node->spellBook.next;
        }
        node->spellBook.next = next;
    }

    next->spellBook.value = spell;
    next->spellBook.next = NULL;

    return spellBook;
}
void freeSpellBook(struct ParseNode* node) {
    if (!node) return;

    struct ParseNode* next;
    while (node) {
        next = node->spellBook.next;
        if (node->spellBook.value->type == PN_spell) {
            freeSpell(node->spellBook.value);
        } else if (node->spellBook.value->type == PN_label) {
            freeLabel(node->spellBook.value);
        } else {
            //Error
            char temp[256];
            sprintf(temp, "\nfreeSpellBook :: Unknown type of spellbook entry (%d) ", node->type);
            strcat(parseErrors, temp);
        }

        free(node); parseNodes--;
        node = next;
    }
}

struct ParseNode* paramList(struct ParseNode* paramList, struct ParseNode* param) {
    //strcat(parseErrors, "pl ");
    parseNodes++;

    struct ParseNode* next = (struct ParseNode*)malloc(sizeof(struct ParseNode));
    next->type = PN_paramList;
    if (paramList == NULL) {
        paramList = next;
    } else {
        //Add value to the end of the list
        struct ParseNode* node = paramList;
        while (node->paramList.next) {
            node = node->paramList.next;
        }
        node->paramList.next = next;
    }

    next->paramList.value = param;
    next->paramList.next = NULL;

    return paramList;
}
void freeParamList(struct ParseNode* node) {
    if (!node) return;

    struct ParseNode* next;
    while (node) {
        next = node->paramList.next;
        freeParam(node->paramList.value);
        free(node); parseNodes--;
        node = next;
    }
}

struct ParseNode* spell(char* name, struct ParseNode* params) {
    //strcat(parseErrors, "s ");
    parseNodes++;

    struct ParseNode* result = (struct ParseNode*)malloc(sizeof(struct ParseNode));
    result->type = PN_spell;

    strncpy(result->spell.name, name, 255);
    result->spell.name[255] = '\0';

    result->spell.params = params;

    return result;
}
void freeSpell(struct ParseNode* node) {
    if (!node) return;

    freeParamList(node->spell.params);
    free(node); parseNodes--;
}

struct ParseNode* label(char* name) {
    //strcat(parseErrors, "l ");
    parseNodes++;

    struct ParseNode* result = (struct ParseNode*)malloc(sizeof(struct ParseNode));
    result->type = PN_label;

    strncpy(result->label.name, name, 255);
    result->label.name[255] = '\0';

    return result;
}
void freeLabel(struct ParseNode* node) {
    if (!node) return;

    free(node); parseNodes--;
}

struct ParseNode* param(ParamType type) {
    //strcat(parseErrors, "p ");
    parseNodes++;

    struct ParseNode* result = (struct ParseNode*)malloc(sizeof(struct ParseNode));
    result->type = PN_param;

    result->param.type = type;
    result->param.intval = 0;
    result->param.floatval = 0;
    result->param.strval[0] = '\0';

    return result;
}
struct ParseNode* parami(ParamType type, int intval) {
    struct ParseNode* result = param(type);
    result->param.intval = intval;
    return result;
}
struct ParseNode* paramf(ParamType type, float floatval) {
    struct ParseNode* result = param(type);
    result->param.floatval = floatval;
    return result;
}
struct ParseNode* params(ParamType type, char* strval) {
    struct ParseNode* result = param(type);

    strncpy(result->param.strval, strval, 255);
    result->param.strval[255] = '\0';

    return result;
}
void freeParam(struct ParseNode* node) {
    if (!node) return;

    free(node); parseNodes--;
}
