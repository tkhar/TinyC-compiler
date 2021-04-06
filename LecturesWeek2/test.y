/* definitions */
%{
	#include<stdio.h>
	#include<vector>
	int yylex(void);
	int total = 0;
	int n = 0;

	using namespace std;
	vector<int> scores;
%}
%token NAME NUMBER
%union {
			int val_i;
			float val_f;
			// astNode *anode;
		}
%type <val_i> NUMBER
%type <val_i> student;
%%
	/* rules */
database : database student { total = total + $2;
										scores.push_back($2);
									   n++;
									 }
			| ;
student : NAME ':' NUMBER '\n' {$$ = $3; };
%%
/* function definitions*/
int main () {
	yyparse();
	printf("Avg: %f\n", (float)total/n);
}

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}
