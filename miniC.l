%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "AST.h"
#include "miniC.tab.h"
%}



intnum [0-9]+
floatnum [0-9]+.[0-9]+
id [A-Za-z][A-Za-z0-9_]*     
space [ \t\r\n]


%%

int     return INT;
float   return FLOAT;

if return IF;
else return ELSE;
for return FOR;
while return WHILE;
do return DO;
return return RETURN;

"[" |
"]" |
"{" |
"}" |
"(" |
")" |
";" |
"." |
"," |
"=" {return yytext[0];} //character 자체로 token
"\""  {return DQUOT_T;}
"\'"  {return SQUOT_T;}
"\&"  {return AMP_T;}
"-"     return MINUS;
"+"     return PLUS;
"*"     return MULT;
"/"     return DIV;
"<="    return LE;
">="    return GE;
">"     return GT;
"<"     return LT;
"=="    return EQ;
"!="    return NE;

{intnum} {
    yylval.intnum = atoi(yytext);
    return INTNUM;
}
{floatnum} {
    yylval.floatnum = atof(yytext);
    return FLOATNUM;
}
{id}   {
    yylval.id = strndup(yytext, yyleng);
    return ID;
}

{space} { break;}

%%
