/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_MINIC_TAB_H_INCLUDED
# define YY_YY_MINIC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTNUM = 258,
    FLOATNUM = 259,
    ID = 260,
    INT = 261,
    FLOAT = 262,
    MINUS = 263,
    PLUS = 264,
    MULT = 265,
    DIV = 266,
    LE = 267,
    GE = 268,
    EQ = 269,
    NE = 270,
    GT = 271,
    LT = 272,
    IF = 273,
    ELSE = 274,
    FOR = 275,
    WHILE = 276,
    DO = 277,
    RETURN = 278,
    DQUOT_T = 279,
    SQUOT_T = 280,
    AMP_T = 281,
    LOWER_THAN_ELSE = 282
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 38 "miniC.y" /* yacc.c:1909  */

    struct PROGRAM       *ptr_program;
    struct DECLARATION   *ptr_declaration;
    struct IDENTIFIER    *ptr_identifier;
    struct FUNCTION      *ptr_function;
    struct PARAMETER     *ptr_parameter;
    struct COMPOUNDSTMT  *ptr_compoundstmt;
    struct STMT          *ptr_stmt;
    struct ASSIGN        *ptr_assign;
    struct CALL          *ptr_call;
    struct ARG           *ptr_arg;
    struct WHILE_S       *ptr_while_s;
    struct FOR_S         *ptr_for_s;
    struct IF_S          *ptr_if_s;
    struct ID_S          *ptr_id_s;
    struct EXPR          *ptr_expr;
    Type_e type;
    //TODO int, float to char*
    int intnum;
    float floatnum;
    char* id;

#line 105 "miniC.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MINIC_TAB_H_INCLUDED  */
