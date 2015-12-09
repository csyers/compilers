%option noyywrap
%{
#include <math.h>
#include <stdlib.h>
#include "token.h" 
#include "parser.tab.h"
%}
delim 	[ \t\n\r]
ws 	{delim}+
digit 	[0-9]
letter 	[a-zA-Z]
id 	({letter}|_)({letter}|{digit}|_)*
number	{digit}+
string  \"([^\"\n]|\\\")*\"
com1 	\/\/.*$
com2    \/\*([^*]|[\r\n]|(\*+([^*\/]|[\r\n])))*\*+\/
chr_lit	\'(((\\)?[^\\])|(\\\\))\'
%%
{ws}			{/* do nothing */}
{com1}			{/* do nothing */}
{com2}			{/* do nothing */}
array			{return TOKEN_ARRAY;}
boolean			{return TOKEN_BOOLEAN;}
else			{return TOKEN_ELSE;}
false			{return TOKEN_FALSE;}
for			{return TOKEN_FOR;}
function		{return TOKEN_FUNCTION;}
if			{return TOKEN_IF;}
print			{return TOKEN_PRINT;}
return			{return TOKEN_RETURN;}
true			{return TOKEN_TRUE;}
void			{return TOKEN_VOID;}
"+"			{return TOKEN_ADD;}
"-"			{return TOKEN_SUB;}
"*"			{return TOKEN_MUL;}
"/"			{return TOKEN_DIV;}
"%"			{return TOKEN_MOD;}
"^"			{return TOKEN_EXP;}
"--"			{return TOKEN_DECREMENT;}
"++"			{return TOKEN_INCREMENT;}
">"			{return TOKEN_GT;}
">="			{return TOKEN_GE;}
"<"			{return TOKEN_LT;}
"<="			{return TOKEN_LE;}
"=="			{return TOKEN_EQ;}
"!="			{return TOKEN_NE;}
"="			{return TOKEN_ASSIGN;}
"&&"			{return TOKEN_AND;}
"||"			{return TOKEN_OR;}
":"			{return TOKEN_COLON;}
";"			{return TOKEN_SEMICOLON;}
"("			{return TOKEN_L_PAREN;}
")"			{return TOKEN_R_PAREN;}
"["			{return TOKEN_L_BRACKET;}
"]"			{return TOKEN_R_BRACKET;}
"{"			{return TOKEN_L_BRACE;}
"}"			{return TOKEN_R_BRACE;}
","			{return TOKEN_COMMA;}
"!"			{return TOKEN_LOGICAL_NOT;}
string			{return TOKEN_STRING;}
{string}		{edit_string(yytext); if(strlen(yytext)>255){fprintf(stderr,"scan error: string too long: %s\n",yytext); exit(1);} else {return TOKEN_STRING_LITERAL;}}
{number}		{return TOKEN_INTEGER_LITERAL;}
{chr_lit}		{edit_string(yytext); return TOKEN_CHARACTER_LITERAL;}
char			{return TOKEN_CHAR;}
integer			{return TOKEN_INTEGER;}
{id}			{if(strlen(yytext)>256){fprintf(stderr,"scan error: identifier too long: %s\n",yytext); exit(1);} else {return TOKEN_IDENT;}}
.			{fprintf(stderr,"scan error: bad character: %c\n",yytext[0]); exit(1);}
%%


