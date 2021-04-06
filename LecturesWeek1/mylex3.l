%{
#include<stdio.h>
%}
%x a
%%
test {printf("matched 2 %s\n",yytext);
		BEGIN a;}
<a>ing {printf("matched ing%s\n", yytext);
			BEGIN 0;}
er {printf("match 3\n"); }
(\/\*)(.)*(\*\/) {printf("comment\n");}
%%
