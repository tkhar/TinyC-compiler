%{
	#include<stdio.h>
	void yyerror(char *);
	int yylex(void);
	int sym[26]; // initialized to 0
%}

%token INTEGER VARIABLE
%left '+' '-'
%left '*' '/'
%%

program :
		   program statement '\n'
			|
			;

statement:
			expr							{ printf("%d\n", $1); }
			| VARIABLE '=' expr		{ sym[$1] = $3; }
			;

expr:
			INTEGER
			| VARIABLE					{ $$ = sym[$1]; }
			| expr '+' expr			{ $$ = $1 + $3; }
			| expr '-' expr			{ $$ = $1 - $3; }
			| expr '*' expr 			{ $$ = $1 * $3; }
			| expr '/' expr			{ $$ = $1 / $3; }
			| '(' expr ')'				{ $$ = $2; }
			;

%%

void yyerror(char *a) {
	fprintf(stderr, "%s\n", a);
}

int main (void) {
	yyparse();
	return 0;
}
