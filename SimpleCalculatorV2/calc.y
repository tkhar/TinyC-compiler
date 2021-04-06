%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<stdarg.h> // variadic functions
	
	/* prototypes */
	nodeType *opr(int oper, int nops, ...);
	nodeType *id(int i);
	nodeType *con(int value);
	void freeNode(nodeType *p);
	int ex(nodeType *p);
	int yylex(void);

	void yyerror (char *s);
	int sym [26]; 		/* symbol table */
%}

// determines YYSTYPE
%union {
	int iValue;
	char sIndex;
	nodeType *nPtr;
};

// declare variables and terminals and accosiate them
// with types in YYSTYPE
// specify the variables and tokens in order of precedence
%token <iValue> INTEGER
%token <sIndex> VARIABLE
%token WHILE IF PRINT
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list
%%

program: 
	function				{exit(0);}
	;

function:
	function stmt		{ ex($2); freeNode($2); }
	| /* NULL */
	;

stmt:
		';'											{ $$ = opr(';', 2, NULL, NULL); }
	|	expr ';'										{ $$ = $1; }
	|	PRINT expr ';'								{ $$ = opr(PRINT, 1, $2); }
	| 	VARIABLE '=' expr ';'					{ $$ = opr('=', 2, id($1), $3); }
	|	WHILE '(' expr ')' stmt					{ $$ = opr(WHILE, 2, $3, $5); }
	| 	IF '(' expr ')' stmt %prec IFX 		{ 
	| 	IF '(' expr ')' stmt ELSE stmt
	| 	'{' stmt_list '}'
	;

%%
