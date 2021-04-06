%{
	#include<stdio.h>
	#include<stdlib.h>
	#include "y.tab.h"
	extern void yyerror(char *);
%}
%%
[a-zA-Z]+ {
				return (NAME);
			 }
[0-9]+	 {
				yylval.val_i = atoi(yytext);
				return (NUMBER);
			 }
[:\n]     {
				return (*yytext);
			 }
[ \t]		;
.			 {
				yyerror(yytext);
			 }
%%
int yywrap() {
	return 1;
}
