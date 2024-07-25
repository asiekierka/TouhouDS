%{

#include <nds.h>
#include <stdio.h>
#include "parser_c.h"

int yylex(void);
void yyerror(char*);

%}

%union {
	int intval;
	float floatval;
	char* strval;

    struct ParseNode* node;
};

%token DEFINE
%token <intval> SPELL OBJECT BOSS ANIMATION CONVERSATION
%token <strval> VARIABLE LABEL IDENTIFIER COMPLEX_IDENTIFIER RANDOM
%token <intval> INT
%token <floatval> FLOAT
%token <strval> STRING
%token NL

%type <node> define_list
%type <node> define
%type <intval> spell_type
%type <node> spellbook
%type <node> label
%type <node> spell
%type <node> spell_param_list
%type <node> param

%%

define_list
	:                       { $$ = defines; }
	| error NL              { $$ = defines; }
	| define_list NL        { $$ = defines; }
	| define_list define    { $$ = defines = defineList(defines, $2); }
	| define_list error NL  { $$ = defines; }
	;

define
    : DEFINE spell_type IDENTIFIER NL spellbook NL { $$ = define($2, $3, $5); clearStringPool(); }
    | DEFINE spell_type IDENTIFIER NL spellbook    { $$ = define($2, $3, $5); clearStringPool(); }
    | DEFINE error NL NL                           { $$ = NULL; }
    ;

spell_type
    : SPELL     	{ $$ = SPELL; }
    | OBJECT        { $$ = OBJECT; }
    | BOSS          { $$ = BOSS; }
    | ANIMATION     { $$ = ANIMATION; }
    | CONVERSATION  { $$ = CONVERSATION; }
    ;

spellbook
    : spell	NL      	 { $$ = spellBook(NULL, $1); }
    | label NL           { $$ = spellBook(NULL, $1); }
    | error NL           { $$ = NULL; }
    | spellbook spell NL { $$ = spellBook($1, $2); }
    | spellbook label NL { $$ = spellBook($1, $2); }
    | spellbook error NL { $$ = $1; }
    ;

label
    : LABEL   { $$ = label($1); }
    ;

spell
    : IDENTIFIER					{ $$ = spell($1, NULL); }
    | IDENTIFIER spell_param_list   { $$ = spell($1, $2); }
    | IDENTIFIER error              { $$ = NULL; }
    ;

spell_param_list
    : param						{ $$ = paramList(NULL, $1); }
    | spell_param_list param	{ $$ = paramList($1, $2); }
    ;

param
    : VARIABLE           { $$ = params(PT_variable, $1); }
    | IDENTIFIER         { $$ = params(PT_identifier, $1); }
    | COMPLEX_IDENTIFIER { $$ = params(PT_identifier, $1); }
    | RANDOM             { $$ = params(PT_random, $1); }
    | FLOAT				 { $$ = paramf(PT_float, $1); }
    | INT				 { $$ = parami(PT_int, $1); }
    | STRING			 { $$ = params(PT_string, $1); }
    ;

%%
