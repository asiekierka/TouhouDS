/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DEFINE = 258,
     SPELL = 259,
     OBJECT = 260,
     BOSS = 261,
     ANIMATION = 262,
     CONVERSATION = 263,
     VARIABLE = 264,
     LABEL = 265,
     IDENTIFIER = 266,
     COMPLEX_IDENTIFIER = 267,
     RANDOM = 268,
     INT = 269,
     FLOAT = 270,
     STRING = 271,
     NL = 272
   };
#endif
/* Tokens.  */
#define DEFINE 258
#define SPELL 259
#define OBJECT 260
#define BOSS 261
#define ANIMATION 262
#define CONVERSATION 263
#define VARIABLE 264
#define LABEL 265
#define IDENTIFIER 266
#define COMPLEX_IDENTIFIER 267
#define RANDOM 268
#define INT 269
#define FLOAT 270
#define STRING 271
#define NL 272




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 12 "thds.yy"
typedef union YYSTYPE {
	int intval;
	float floatval;
	char* strval;

    struct ParseNode* node;
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 80 "thds.tab.hh"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



