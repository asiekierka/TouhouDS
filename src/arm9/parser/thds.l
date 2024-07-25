%{

#include <stdlib.h>	
#include "parser_c.h"
#include "thds.tab.hh"

int scriptGetInput(char* data, int dataL);
void yyerror(const char* str);
void str(char** dst, const char* src);
void trimStr(char** dst, const char* src, int left, int right);

#undef YY_INPUT
#define YY_INPUT(data, retval, dataL) (retval = scriptGetInput(data, dataL))
#define YY_NO_UNPUT

char* stringPool[1024];
int stringPoolIndex = 0;

%}

D                   [0-9]
L			        [a-zA-Z_]
I					(-)?{D}*
F					(-)?{D}*\.{D}+
ID					{L}({L}|{D})*

%%

"define"            { return DEFINE; }
"spell"             { return SPELL; }
"object"            { return OBJECT; }
"boss"              { return BOSS; }
"animation"         { return ANIMATION; }
"conversation"      { return CONVERSATION; }

\$\$    	       	{ str(&yylval.strval, yytext); return VARIABLE; }
\$({D})*	       	{ str(&yylval.strval, yytext); return VARIABLE; }
:{ID}:				{ str(&yylval.strval, yytext); return LABEL; }
{ID}	         	{ str(&yylval.strval, yytext); return IDENTIFIER; }
({ID}\.)?{ID}		{ str(&yylval.strval, yytext); return COMPLEX_IDENTIFIER; }

~({F}|{I}):({F}|{I})(;({F}|{I}))?		{ str(&yylval.strval, yytext); return RANDOM; }

{F}					{ yylval.floatval = atof(yytext); return FLOAT; }
{I}					{ yylval.intval = atoi(yytext); return INT; }
\"(\.|[^\\"])*\"	{ trimStr(&yylval.strval, yytext, 1, 1); return STRING; }

\n                  { parserLine++; return NL; }
#[^\n]*\n           { parserLine++; /* ignore comments */ }
[ \r\t\v\f]         {               /* ignore whitespace */ }
.                   { yyerror("Invalid Character"); }

%%

int yywrap() {
	return 1;
}
void str(char** dst, const char* src) {
    *dst = stringPool[stringPoolIndex++] = strdup(src);    
}
void trimStr(char** dst, const char* src, int left, int right) {
    int srcL = strlen(src);
    int dstL = MAX(0, srcL - left - right);
    
    *dst = stringPool[stringPoolIndex++] = (char*)malloc(dstL+1);    
    strncpy(*dst, src + left, dstL);
    (*dst)[dstL] = '\0';
}
void clearStringPool() {
    int n;
    for (n = 0; n < stringPoolIndex; n++) {
        free(stringPool[n]);
    }
    stringPoolIndex = 0;
}
