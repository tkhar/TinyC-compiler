%{
#include<stdio.h>
int numOfLines=0;
int numOfWords=0;
%}
LINE \n
WORD [a-zA-Z0-9]*
%%
{LINE} {numOfLines++;}
^{WORD} {numOfWords++; ECHO; printf("\n");}
{WORD} {numOfWords++;}
. {printf(""); }
%%
int main(int argc, char *argv[]){
	yyin = fopen(argv[1], "r");
	yylex();
	printf("%d %d\n",numOfWords, numOfLines);
	fclose(yyin);
}

int yywrap() {
	return 1;
}
