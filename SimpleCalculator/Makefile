# Makefile for simple calculator
# Haris Themistoklis
#
# Taken from notes by Tom Niemann

PROG = sc
LEX = lex.yy.c
YACC = y.tab.c
LEXSOURCE = sc.l
YACCSOURCE = sc.y

PROG: clean $(LEX) $(YACC)
	echo "Making calculator"
	gcc -o $(PROG) $(LEX) $(YACC)

$(LEX):
	echo "Making lex program"
	lex $(LEXSOURCE)

$(YACC):
	echo "Making yacc program"
	yacc -d $(YACCSOURCE)


clean:
	rm -rf $(LEX)
	rm -rf $(YACC)
	rm -rf ./a.out
