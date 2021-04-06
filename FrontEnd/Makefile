# Makefile for FrontEnd - Long Assignment 1
#
# Author: Haris Themistoklis
# Date: 9/21/2019

LEX=minic.l
YACC=minic.y
LEXPROG=lex.yy.c
YACCPROG=y.tab.c
ASTPROG=ast.c
EXEC=createASTandIR

IRGEN=ast_to_llvm_ir.c

DEBUG=-t -v

main: clean $(YACCPROG) $(LEXPROG)
	echo "Building main executable"
	c++ -g `llvm-config --cxxflags --ldflags --libs core` $(YACCPROG) $(LEXPROG) $(ASTPROG) $(IRGEN) -o $(EXEC)

$(LEXPROG):
	echo "Building lex c program"
	lex $(LEX)

$(YACCPROG):
	echo "Building yacc c program"
	yacc -d $(DEBUG) $(YACC)

clean:
	rm -rf $(LEXPROG)
	rm -rf $(YACCPROG)
	rm -rf $(EXEC)
	rm -rf y.tab.h
	rm -rf y.output
	rm -rf ./a.out
	rm -rf core
	rm -rf resultingIR.ll 
