%{
	#include<stdlib.h>
	void yyerror (char *);
	#include "y.tab.h"
	#ifndef YYSTYPE
	#define YYSTYPE int
	#endif
	extern YYSTYPE yylval;
%}

%%
	/* variables */
[a-z]			{
					yylval = *yytext - 'a';
					return VARIABLE;
				}
	/* integers */
[0-9]+		{
					yylval = atoi(yytext);
					return INTEGER;
				}
	/* operators */
[-+()=/*\n] {	return *yytext; }
	
	/* skip whitespace */
[ \t]			;

	/* anything else is an error */
.				yyerror("invalid character");
%%

int yywrap(void) {
	return 1;
}
