%{
#include<stdio.h>
%}
%%
int {printf("found int\n");}
void {printf("found int\n");}
extern {printf("found extern\n");}
%%

int main(int argc, char *argv[]){
	yyin = fopen(argv[1], "r");
	yylex();
	fclose(yyin);
}
int yywrap(){
	return 1;
}
