# Makefile for Short Assignment 1 - lex
#
# Author: Haris Themistoklis
# Date: 9/21/2019

LEX=sa1.l
LEXPROG=lex.yy.c
EXEC=sa1

main: clean $(LEXPROG)
	echo "Building main"
	gcc $(LEXPROG) -o $(EXEC)

$(LEXPROG):
	echo "Building lex c program"
	lex $(LEX)

clean:
	rm -rf $(LEXPROG)
	rm -rf $(EXEC)
	rm -rf ./a.out
