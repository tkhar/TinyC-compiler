/*
	minic.l - A (F)Lex program which performs lexical analysis for the
	MiniC langauge.

	Author: Haris Themistoklis
	CS57 - Compilers

	10 / 16 / 2019

	README
	-------------
	ECHO; commands are kept because they provide guidelines to the
	progress of the parser and are great tools for understanding
	its function and even debuggin it.
	-------------
*/
%{
	#include <stdlib.h>
	#include <string.h>
	#include "ast.h"
	#include "y.tab.h"

	void yyerror(char *);
%}

%%

0										{
											ECHO;
											yylval.iValue = atoi(yytext);
											return INTEGER;
										}
[1-9][0-9]*							{ 
											ECHO;
											yylval.iValue = atoi(yytext);
											return INTEGER;
										}
[-()<>=+*/;{}]						{
											ECHO;
											return *yytext;
										}

">="									{ 	ECHO;
											return GE;
										}
"<="									{	
											ECHO; 
											return LE; 
										}
"=="									{	
											ECHO; 
											return EQ;
										}
"!="									{  
											ECHO; 
											return NE; 
										}
"while"								{	
											ECHO;
											return WHILE; 
										}
"if"									{ 
											ECHO; 
											return IF; 
										}
"else"								{	ECHO; 
											return ELSE; 
										}
"extern"								{	
											ECHO; 
											return EXTERN; 
										}
"int"									{	
											ECHO;
											return INT; 
										}
"void"								{	ECHO;
											return VOID; 
										}
"read"								{	
											ECHO; 
											return READ;
										}
"print"								{	
											ECHO;
											return PRINT; 
										}
"return" 							{	
											ECHO;
											return RETURN;
										}

[a-zA-z][a-zA-z0-9]*				{
											ECHO;
											// Just yylval.varName = yytext doesn't work!!
											yylval.varName = strdup(yytext);
											return id;
										}

[ \t\n]+								; // ignore whitespace

.										{ 
											ECHO;
											yyerror("Unknown character.");
										}

%%

int yywrap(void) {
	return 1;
}
