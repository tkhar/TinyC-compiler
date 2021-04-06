/*
	minic.y - A yacc parser file which does syntax analysis for 
	the MiniC language

	Author: Haris Themistoklis
	CS57 - Compilers

	10 / 16 / 2019

	README
	-----------
	A problem with the module developed below is that in case of a syntax error the program
	does not free the memory it has allocated. It only does so when the compilation is
	successfuly
	-----------

*/
%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <vector>
	#include <stack>
	#include "ast.h"

	#define ANSI_COLOR_RESET   "\x1b[0m"

	using namespace std;

	// We are using a stack of symbol tables which we populate
	// as we encounter the declaration of variables and which
	// we use for the semantic analysis we perform.
	vector< vector<char *> > symbol_table_stack; 
	bool compilation_success = true;

	void yyerror (char *s); // declaration of yyerror
	void astToLLVMIR (astNode *root); // declaration of IR building function
%}

%union {
	int iValue;					// integer value
	char *varName; 			// variable name
	vector <astNode *> *stmt_list; // statement list
	astNode*	nodePointer;	// node pointer
};

%token <iValue> INTEGER
%token <varName> id
%token WHILE IF EXTERN INT READ PRINT VOID RETURN
%nonassoc IFX
%nonassoc ELSE					// give ELSE a higher precedence to get rid of if-else ambiguity

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nodePointer> program extern function arg stmt var const term expr b_expr u_expr r_expr read_call
%type <stmt_list> block_stmt

%%
program:
	extern extern function									{ 
																		/* create program node by putting in the external function nodes
																		 * and the main function node */
																		$$ = createProg ($1, $2, $3);

																		if (compilation_success) {
																			printf ("\nPrinting Abstract Syntax Tree:\n");
																			printNode ($$);
																			printf ("Compilation successful!\n");
																		}
																		else {
																			printf ("Compilation failed!\n");
																		}

																		// time to build the LLVM IR!
																		astToLLVMIR ($$);

																		// Free the tree!
																		freeNode($$);
																		exit(0); 
														    		}
	;

extern: 
	EXTERN VOID PRINT '(' INT ')' ';'					{
																		/* create extern node.*/
																		$$ = createExtern ("print");
																	}
	| EXTERN INT READ '(' VOID ')' ';'					{
																		/* create extern node.*/
																		$$ = createExtern ("read");
																	}
	;

function: 
	INT id '(' arg ')' stmt									{	 
																		/* create function node, providing the name, the parameter, if one
																		 * exists and the body */
																		$$ = createFunc ($2, $4, $6);
																	}
	;

arg: 
	INT id														{	
																		/* create a declaration node, just with the name of the variable
																		 * and start making the stack of symbol tables*/
																		// Also, make the first symbol table and push the argument in it
																		vector<char *> first_symbol_table;
																		first_symbol_table.push_back($2);
																		symbol_table_stack.push_back(first_symbol_table);
																		$$ = createVar ($2);
																	}
	|																{
																		/* There are no arguments to our function */
																		$$ = NULL;
																	}
	;

stmt:
	INT id ';'													{
																		/* create declaration node with just the name of the variable
																			and continue with the stack of symbol tables */
																		// Add $2 to the symbol table that is on top of our stack.
																		// While doing that, detect if we have duplicate
																		// declarations!
																		printf ("\nDeclaration: %s\n", $2);
																		vector<char *> &current_symbol_table = symbol_table_stack.back();
																		printf ("\nChecking if variable has been declared again\n");
																		for (vector<char *>::iterator it = current_symbol_table.begin();
																				it != current_symbol_table.end(); ++it) {
																				if (strcmp((*it), $2) == 0) {
																					printf ("\nSEMANTIC ERROR: Variables can only be declared once!\n");
																					// freeing cannot happen here! We must return to the top
																					// of the tree!
																					compilation_success = false;
																				}
																		}
																		printf ("Checking concluded\n");
																		current_symbol_table.push_back ($2);
																		printf("\n--------------\n");
																		printf("Now the symbol table stack looks like: \n");
																		for (int i=0; i<symbol_table_stack.size(); i++){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				printf("%s ",symbol_table_stack[i][j]);
																			}
																			printf("\n");
																		}
																		printf("------------\n");

																		$$ = createDecl ($2);
																	}
	| var '=' expr ';'										{ 
																		/* create assignment node with the variable and the expression */
																		$$ = createAsgn ($1, $3);
																	}
	| var '=' read_call ';'									{
																		/* create assignment node with the variable and the call-node
																		 * created from the READ function. */
																		$$ = createAsgn ($1, $3);
																	}
	| IF '(' r_expr ')' stmt %prec IFX					{
																		/* create an if-node pointing to the condition and the body with
																		 * else-body = NULL (happens by default) */
																		$$ = createIf ($3, $5);
																	}
	| IF '(' r_expr ')' stmt ELSE stmt					{
																		/* create an if-node pointing to the condition, the main body and
																		 * the else-body */
																		$$ = createIf ($3, $5, $7);
																	}
	| WHILE '(' r_expr ')' stmt							{
																		/* create a while-node pointing to the condition and body */
																		$$ = createWhile ($3, $5);
																	}
	| block_begin block_stmt block_end					{
																		/* create a block-statement node pointing to a vector of
																		 * statements that is returned by block_stmt ($2) */
																		// Remove the last layer of the stack of symbol tables.
																		$$ = createBlock ($2);
																	}
	| RETURN '(' expr ')' ';'								{
																		/* create a return-statement node pointing to the expression being
																		 * returned. */
																		$$ = createRet ($3);
																	}
	| PRINT '(' id ')' ';'								   {
																		/* create a call-statement node pointing to the function name
																		 * (PRINT) and
																		 * to the VARIABLE being used as a parameter. */
																		// We must also check if that variable has been declared!
																		astNode *printDecl = createDecl ($3);
																		bool hasBeenDeclared = false, stop = false;
																		printf("\nChecking if variable %s has been declared\n", $3);
																		for (int i=symbol_table_stack.size()-1; i>=0; i--){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				if (strcmp(symbol_table_stack[i][j], $3) == 0) {
																					hasBeenDeclared = true;
																					stop = true;
																					break;
																				}
																			}
																			if (stop) {
																				break;
																			}
																		}
																		printf("------- Check concluded -------\n");
																		if (!hasBeenDeclared) {
																			printf("\nSEMANTIC ERROR: variable %s has not been declared!\n",$3);
																			// free-ing the memory cannot happen here! It must happen at
																			// the end!
																			compilation_success = false;
																		}
																		if (printDecl == NULL)
																			yyerror ("\nError in the print() statement!\n");
																		$$ = createCall ("print", printDecl);
																	}
	;

block_begin:
	'{'															{
																		/*
																		*	When a block statement begins, we must make a new symbol table
																		*	and populate it with the newly declared variables.
																		*/
																		vector <char *> new_symbol_table;
																		symbol_table_stack.push_back(new_symbol_table);
																		
																		// print the stack of symbol tables
																		printf("\n--------------\n");
																		printf("Now the symbol table stack looks like: \n");
																		for (int i=0; i<symbol_table_stack.size(); i++){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				printf("%s ",symbol_table_stack[i][j]);
																			}
																			if (symbol_table_stack[i].size() == 0) {
																				printf("[]");
																			}
																			printf("\n");
																		}
																		printf("------------\n");
																	}
	;

block_end:
	'}'															{
																		// when a block statement ends, we have to pop its respective
																		// symbol table from our stack so that we can correctly keep track
																		// of the scopes of our variables.
																		printf ("\nPopping from symbol table stack.\n");
																		symbol_table_stack.pop_back();

																		printf("--------------\n");
																		printf("Now the symbol table stack looks like: \n");
																		for (int i=0; i<symbol_table_stack.size(); i++){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				printf("%s ",symbol_table_stack[i][j]);
																			}
																			if (symbol_table_stack[i].size() == 0) {
																				printf("[]");
																			}
																			printf("\n");
																		}
																		printf("------------\n");
																	}
	;

block_stmt:
	stmt block_stmt											{
																		/* populate the vector with the statement nodes */
																		($2) -> push_back($1);
																		$$ = $2;
																	}
	| stmt														{
																		/* initialize a vector of ast-nodes and put the first statement
																		 *	in it */
																		vector<astNode *> *list_of_statements = new vector<astNode *> ();
																		list_of_statements -> push_back($1);
																		$$ = list_of_statements;
																	}
	;																

read_call:
	READ '(' ')'												{
																		/* create a call-statement node pointing to the function name
																		 * (READ) with a NULL parameter. */
																		$$ = createCall("read");
																	}
	;

expr:
	term															{
																		/* just pass the node already created on */
																		$$ = $1;
																	}
	| b_expr														{
																		/* just pass on the node already created */
																		$$ = $1;
																	}
	| u_expr														{
																		/* just pass on the node already created */
																		$$ = $1;
																	}
	;

b_expr:
	term '+' term												{	
																		/* create binary expression-node with the two terms and the
																		 * operator. */
																		 $$ = createBExpr ($1, $3, add);
																	}
	| term '-' term											{	
																		/* create binary expression-node with the two terms and the
																		 * operator. */
																		 $$ = createBExpr ($1, $3, sub);
																	}
	| term '*' term											{
																		/* create binary expression-node with the two terms and the
																		 * operator. */
																		 $$ = createBExpr ($1, $3, mul);
																	}
	| term '/' term											{
																		/* create binary expression-node with the two terms and the
																		 * operator. */
																		 $$ = createBExpr ($1, $3, divide);
																	}
	;

u_expr:
	'-' term	%prec UMINUS									{
																		/* create unary expression-node with the operator and the term */
																		$$ = createUExpr ($2, uminus); 
																	}
	;

r_expr:
	expr '>' expr												{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 $$ = createRExpr ($1, $3, gt); 
																	}
	| expr '<' expr											{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 $$ = createRExpr ($1, $3, lt);
																	}
	| expr EQ expr												{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 $$ = createRExpr ($1, $3, eq);
																	}
	| expr LE expr												{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 $$ = createRExpr ($1, $3, le);
																	}
	| expr GE expr												{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 $$ = createRExpr ($1, $3, ge);
																	}
	| expr NE expr												{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 $$ = createRExpr ($1, $3, neq);
																	}
	;

term:
	var															{ 
																		/* just pass on the node already created */ 
																		$$ = $1; 
																	}
	| const														{
																		/* just pass on the node already created */
																		$$ = $1; 
																	}
	;

var:
	id																{
																		/* create variable node with just the name of the variable. Check
																		 * if the variable has been declared by using the symbol table
																		 * stack. */
																		 bool hasBeenDeclared = false, stop = false;
																		 printf("\nChecking if variable %s has been declared\n", $1);
																		 for (int i=symbol_table_stack.size()-1; i>=0; i--){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				if (strcmp(symbol_table_stack[i][j], $1) == 0) {
																					hasBeenDeclared = true;
																					stop = true;
																					break;
																				}
																			}
																			if (stop) {
																				break;
																			}
																		 }
																		 printf("------- Check concluded -------\n");
																		 if (!hasBeenDeclared) {
																			printf("\nSEMANTIC ERROR: variable %s has not been declared!\n",$1);
																			// free-ing the memory cannot happen here! It must happen at
																			// the end!
																			compilation_success = false;
																		 }

																		 $$ = createVar($1);
																	}
	;
const:
	INTEGER														{
																		/* create a constant node with the value of the integer */
																		$$ = createCnst($1);
																	}
	;
%%

/* prints out an error message to the console */
// TOOD: Add more functionality
void yyerror(char *s) {
	fprintf(stdout,"\nCompilation Error: %s\n",s);
}

int yylex (void);

int main (void) {
	yyparse();
	return 0;
}
