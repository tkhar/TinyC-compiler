%{
#include<stdio.h>
%}
%%
test {printf("matched: test");}
a* {printf("found: a's");}
. 
%%
int main(int argc, char *argv[]){
	yyin = fopen(argv[1], "r");
	yylex();
	fclose(yyin);
}
int yywrap(){
	return 1;
}
