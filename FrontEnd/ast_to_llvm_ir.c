/*
 * Building the LLVM IR for the miniC language.
 *
 * Author: Haris Themistoklis
 *
 * Date: 10/31/19
 */
#include <stdio.h>
#include <inttypes.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <iostream>
#include <cassert>
#include <cstring>
#include "ast.h"

using namespace std; 

// maps variable names to stacks of alloca instructions which
// are built based on the current scope
map <string, stack<LLVMValueRef> >CurrVarToAlloca;
// the current basic block we are examining.
LLVMBasicBlockRef currBasicBlock;
// a dummy marker instruction in the first block which tells us where
// to add our alloca instructions
LLVMValueRef allocaEndInstr; 

// the read and write function declarations
LLVMValueRef readFunc, printFunc;
// our main function
LLVMValueRef func;

const char *printName = "print";
const char *readName = "read";

void handleBlock (astNode *, LLVMBuilderRef& ); 
LLVMValueRef handleVar (astNode *, LLVMBuilderRef& ); 
LLVMValueRef handleConst (astNode *, LLVMBuilderRef& );
LLVMValueRef handleUExpr (astNode *, LLVMBuilderRef& );
LLVMValueRef handleBExpr (astNode *, LLVMBuilderRef& );
LLVMValueRef handleRExpr (astNode *, LLVMBuilderRef& );
LLVMValueRef handleCall (astNode *, LLVMBuilderRef& );
void handleRet (astNode *, LLVMBuilderRef& ); 
void handleWhile (astNode *, LLVMBuilderRef& ); 
void handleIf (astNode *, LLVMBuilderRef& ); 
void handleAsgn (astNode *, LLVMBuilderRef& ); 
void handleDecl (astNode *, LLVMBuilderRef& ); 

/*
 * Handles a astVar node.
 * It searches in our map of current variable names <-> alloca instructions and
 * performs a load, which it then returns.
 *
 * CAUTION: It works the same on astDecl and astVar nodes, as they both contain 
 * a pointer to the variable name!
 */
LLVMValueRef handleVar (astNode *varNode, LLVMBuilderRef& builder) {
	assert(varNode != NULL && (varNode->type == ast_var || (varNode->type == ast_stmt && varNode->stmt.type == ast_decl)));
	printf("\nhandleVar() is called!");
	string varName;
	if (varNode->type == ast_var) {
		// get an std::string version of the variable name
		varName = varNode->var.name;
	}
	else if (varNode->type == ast_stmt && varNode->stmt.type == ast_decl) {
		varName = varNode->stmt.decl.name;
	}
	// check if this variable has been allocated through an alloca command.
	// We are in trouble if not!
	map <string, stack<LLVMValueRef> >::iterator found = CurrVarToAlloca.find(varName);
	// if we found the command...
	if (found != CurrVarToAlloca.end()) {
		// get the alloca instruction and perform the load
		LLVMValueRef varAlloca = CurrVarToAlloca[varName].top();
		LLVMValueRef loadVar = LLVMBuildLoad2(builder, LLVMInt32Type(), varAlloca, "");	
		return loadVar;
	}
	else {
		printf ("\nERROR! Variable used with no alloca instruction!\n");
		exit (0);
	}
	return NULL; // should never be reached!
}

/* Makes and returns an LLVM Constant */
LLVMValueRef handleConst (astNode *constNode, LLVMBuilderRef& builder) {
	assert(constNode != NULL && constNode->type == ast_cnst);
	printf("\nhandleConst() is called!\n");
	printf("Const value: %d\n", constNode->cnst.value);
	return LLVMConstInt (LLVMInt32Type(), constNode->cnst.value, 1);
}

/* 
 * Handles unary expressions.
 */
LLVMValueRef handleUExpr (astNode *uexpr, LLVMBuilderRef& builder) {
	assert(uexpr != NULL && uexpr->type == ast_uexpr);
	printf("\nhandleUExpr() is called!\n");
	// - (term)
	// term can be a variable or a constant
	// In both cases, we are getting the operand and making a unary
	// expression instruction
	// Afterwards, we return the resulting instruction.
	if (uexpr->uexpr.expr->type == ast_var) {
		LLVMValueRef operand = handleVar (uexpr->uexpr.expr, builder);
		return LLVMBuildNeg (builder, operand, "");
	} 
	else if (uexpr->uexpr.expr->type == ast_cnst) {
		LLVMValueRef operand = handleConst (uexpr->uexpr.expr, builder);
		return LLVMBuildNeg (builder, operand, "");
	}
	else {
		printf ("\n ERROR. Unary expression operand of unknown type!\n");
		exit (0);
	}
	return NULL; // never reached
}

/* 
 * Handles binary expressions
 */
LLVMValueRef handleBExpr (astNode *bexpr, LLVMBuilderRef& builder) {
	assert(bexpr != NULL && bexpr->type == ast_bexpr);
	printf("\nhandleBEXpr() is called!\n");
	// get the right hand and left hand sides of expression
	astNode *lhs = bexpr->bexpr.lhs;
	LLVMValueRef lhsOperand;
	astNode *rhs = bexpr->bexpr.rhs;
	LLVMValueRef rhsOperand;
	// get operator
	op_type op = bexpr->bexpr.op;
	
	/* Depending on the type of operand, create different LLVM
	 * Instructions*/
	if (lhs -> type == ast_var) {
		lhsOperand = handleVar (lhs, builder);
	} 
	else if (lhs -> type == ast_cnst) {
		lhsOperand = handleConst (lhs, builder);
	}
	if (rhs -> type == ast_var) {
		rhsOperand = handleVar (rhs, builder);
	} 
	else if (rhs -> type == ast_cnst) {
		rhsOperand = handleConst (rhs, builder);
	}
	
	// depending on the operator type, generate a different LLVM Instruction
	switch (op) {
		case add: 
					 	return LLVMBuildAdd (builder, lhsOperand, rhsOperand, "");
					 
		case sub: 
					 	return LLVMBuildSub (builder, lhsOperand, rhsOperand, "");
					 
		case mul: 
						return LLVMBuildMul (builder, lhsOperand, rhsOperand, "");
					 
		case divide: 
						return LLVMBuildSDiv (builder, lhsOperand, rhsOperand, "");
						 
		default: 
						printf ("\n ERROR! Unrecognizable binary arithmetic operator!\n");
						exit (0);
					
	}
	return NULL; // never reached
}

/*	
 *	Handles relational expression
 */
LLVMValueRef handleRExpr (astNode *rexpr, LLVMBuilderRef& builder) {
	assert(rexpr != NULL && rexpr->type == ast_rexpr);
	printf("\nhandleRExpr() is called\n");
	// get the right hand and left hand sides of expression
	astNode *lhs = rexpr->rexpr.lhs;
	LLVMValueRef lhsOperand;
	astNode *rhs = rexpr->rexpr.rhs;
	LLVMValueRef rhsOperand;
	// get operator
	rop_type op = rexpr->rexpr.op;
	
	/* Depending on the type of operand, create different LLVM
	 * Instructions.
	 * The operands could be variables, constants, unary expressions 
	 * binary expressions
	 */
	if (lhs -> type == ast_var) {
		lhsOperand = handleVar (lhs, builder);
	} 
	else if (lhs -> type == ast_cnst) {
		lhsOperand = handleConst (lhs, builder);
	}
	else if (lhs -> type == ast_uexpr) {
		lhsOperand = handleUExpr (lhs, builder);	
	}
	else if (lhs -> type == ast_bexpr) {
		lhsOperand = handleBExpr (lhs, builder);	
	}
	if (rhs -> type == ast_var) {
		rhsOperand = handleVar (rhs, builder);
	} 
	else if (rhs -> type == ast_cnst) {
		rhsOperand = handleConst (rhs, builder);
	}
	else if (rhs -> type == ast_uexpr) {
		rhsOperand = handleUExpr (rhs, builder);	
	}
	else if (rhs -> type == ast_bexpr) {
		rhsOperand = handleBExpr (rhs, builder);	
	}
	
	// depending on the operator type, generate a different LLVM Instruction
	switch (op) {
		case lt: 
						return LLVMBuildICmp(builder, LLVMIntSLT, lhsOperand, rhsOperand, "");
				 	
		case le: 
						return LLVMBuildICmp(builder, LLVMIntSLE, lhsOperand, rhsOperand, "");
					
		case gt: 
						return LLVMBuildICmp(builder, LLVMIntSGT, lhsOperand, rhsOperand, "");
					
		case ge: 
						return LLVMBuildICmp(builder, LLVMIntSGE, lhsOperand, rhsOperand, "");
					
		case eq: 
						return LLVMBuildICmp(builder, LLVMIntEQ, lhsOperand, rhsOperand, "");
					
		case neq: 
						return LLVMBuildICmp(builder, LLVMIntNE, lhsOperand, rhsOperand, "");
					 
		default: 
						printf ("\n ERROR! Unrecognizable binary relational operator!\n");
						exit (0);
					
	}
	return NULL; // never reached!
}

/* 
 * Handles a call to a function (print() or read())
 * An astCall node will have:
 * 	- char * name;
 * 	- astNode *param; // will be a declaration node in my implementation
 * Returns a pointer to the call instruction created - which will only be needed
 * in case of a call to read()
 */
LLVMValueRef handleCall (astNode *callNode, LLVMBuilderRef& builder) {
	assert(callNode != NULL && callNode->type == ast_stmt && callNode->stmt.type == ast_call);
	printf("\nhandleCall() is called\n");
	char *funcName = callNode->stmt.call.name; // get function name
	if (strcmp(funcName, "read") == 0) {
		printf("\nCalling read() function.\n");
		// for read, param is NULL, so just call the read function with no parameters
		LLVMValueRef params [] = {};
		return LLVMBuildCall(builder, readFunc, params, 0, "");
	}
	else if (strcmp (funcName, "print") == 0) {
		printf("\nCalling print() function.\n");
		// for print(), we have a param, which is a astDecl node!
		astNode *param = callNode->stmt.call.param;
		LLVMValueRef params [1] = { handleVar (param, builder) };
		printf("\nAcquired parameter for print()\n");
		// last parameter must be an empty string!! Also, LLVMBuildCall2 segfaults.
		return LLVMBuildCall(builder, printFunc, params, 1, "");
	}
	else {
		printf ("\n ERROR: Unknown function called!\n");
		exit(0);
	}
	return NULL; // never reached
}

/* 
 * Handles the return statement, which only contains a pointer
 * to an expression by making a return statement
 */
void handleRet (astNode *retNode, LLVMBuilderRef& builder) {
	assert(retNode != NULL && retNode->type == ast_stmt && retNode->stmt.type == ast_ret);
	printf("\n HandleRet() is called!\n");
	if (retNode->stmt.ret.expr->type == ast_bexpr) {
		LLVMBuildRet (builder, handleBExpr( retNode->stmt.ret.expr, builder));
	}
	else if (retNode->stmt.ret.expr->type == ast_uexpr) {
		LLVMBuildRet (builder, handleUExpr (retNode->stmt.ret.expr, builder));
	}
	else if (retNode->stmt.ret.expr->type == ast_cnst) {
		LLVMBuildRet (builder, handleConst (retNode->stmt.ret.expr, builder));
	}
	else if (retNode->stmt.ret.expr->type == ast_var) {
		LLVMBuildRet (builder, handleVar (retNode->stmt.ret.expr, builder));
	}
	else {
		printf ("\n ERROR: Unknown expression type in RETURN node\n");
		exit (0);
	}
}

/* 
 * Handles while node
 * while node:
 * 	astNode *cond -> r_expr
 * 	astNode *body -> block_statement
 *
 *	ATTENTION: Theoretically, one could have
 *		while ( --- ) 
 *			i = i + 1;
 *	, that is the body might not be a block statement, but
 *	we ASSUME that never happens.
 */
void handleWhile (astNode *whileNode, LLVMBuilderRef& builder) {
	assert(whileNode != NULL && whileNode->type == ast_stmt && whileNode->stmt.type == ast_while);
	printf("\n handleWhile() is called!");
	// Creating basic blocks for while-body and for after while ends
	// and for the condition
	LLVMBasicBlockRef cond_BB = LLVMAppendBasicBlock(func, "");
   LLVMBasicBlockRef while_BB = LLVMAppendBasicBlock(func, "");
   LLVMBasicBlockRef final = LLVMAppendBasicBlock(func, "");

	// branch to the condition block!
	LLVMBuildBr (builder, cond_BB);
	LLVMPositionBuilderAtEnd (builder, cond_BB);
	// current basic block = cond_BB!
	currBasicBlock = cond_BB;
	// get the condition instruction
	LLVMValueRef condInstrBefore = handleRExpr (whileNode->stmt.whilen.cond, builder); 
	// conditionally branch
	// based on the condition instruction
	LLVMBuildCondBr(builder, condInstrBefore, while_BB, final);

	// go to the while basic block and build it
	LLVMPositionBuilderAtEnd(builder, while_BB);
	// the current basic block is the while block!
	currBasicBlock = while_BB;
	handleBlock (whileNode ->stmt.whilen.body, builder);
	LLVMBuildBr (builder, cond_BB);

	// head to the final basic block
	LLVMPositionBuilderAtEnd (builder, final);
	// update current basic block variable
	currBasicBlock = final;
	printf("\n----- end of handleWhile() --------\n");
}

/* 
 * Handles if node
 * if node:
 * 	astNode *cond -> r_expr
 * 	astNode *body -> block_statement (!)
 * 	astNode *else-body -> block_statement (!!)
 *	ATTENTION: Theoretically, one could have
 *		if ( --- ) 
 *			i = i + 1;
 *	, that is the body might not be a block statement, but
 *	we ASSUME that never happens.
 *
 *	else-body might also be NULL
 */
void handleIf (astNode *ifNode, LLVMBuilderRef& builder) {
	assert(ifNode != NULL && ifNode->type == ast_stmt && ifNode->stmt.type == ast_if);
	printf("\nhandleIf() is called!\n");
	// get the condition instruction
	LLVMValueRef condInstr = handleRExpr (ifNode ->stmt.ifn.cond, builder); 

	//Creating basic blocks for if and for after if ends	
   LLVMBasicBlockRef if_BB = LLVMAppendBasicBlock(func, "");
   LLVMBasicBlockRef final = LLVMAppendBasicBlock(func, "");

	if (ifNode->stmt.ifn.else_body == NULL) {
		// if no else exists, just go to final basic block in case
		// the condition is false
		LLVMBuildCondBr(builder, condInstr, if_BB, final);
		// go to the if basic block and build it
		LLVMPositionBuilderAtEnd(builder, if_BB);
		// the current basic block is the if block!
		currBasicBlock = if_BB;
		handleBlock (ifNode->stmt.ifn.if_body, builder);
		// put a branch to the final block at the end of the if-block
		LLVMBuildBr (builder, final);
	}
	else {
		// if there is an else, go there in case of the condition being false
		LLVMBasicBlockRef else_BB = LLVMAppendBasicBlock(func, "");
		LLVMBuildCondBr (builder, condInstr, if_BB, else_BB);
		// go to the if basic block and build it
		LLVMPositionBuilderAtEnd (builder, if_BB);
		// the current basic block is the if block!
		currBasicBlock = if_BB;
		handleBlock (ifNode->stmt.ifn.if_body, builder);
		// put a branch to the final block at the end of the if block
		LLVMBuildBr (builder, final);
		// go to the else basic block and build it
		LLVMPositionBuilderAtEnd (builder, else_BB);
		// the current basic block is the else block!
		currBasicBlock = else_BB;
		handleBlock (ifNode->stmt.ifn.else_body, builder);
		// put a branch to the final block at the end of the else-block
		LLVMBuildBr (builder, final);
	}
	// re-position builder at beginning of final basic block
	LLVMPositionBuilderAtEnd (builder, final);
	// update current basic block variable
	currBasicBlock = final;
}

/* 
 * Handles an assignment (like a = 5) by creating a store instruction.
 * astAsgn have the form:
 * 	-lhs: var
 * 	-rhs: expr
 */
void handleAsgn (astNode *asgnNode, LLVMBuilderRef& builder) {
	assert(asgnNode != NULL && asgnNode->type == ast_stmt && asgnNode->stmt.type == ast_asgn);
	printf("\n handleAsgn() is called!\n");
	// get left hand and right hand sides
	astNode *varLHS = asgnNode->stmt.asgn.lhs;
	astNode *exprRHS = asgnNode->stmt.asgn.rhs;
	// load the left hand side variable
	LLVMValueRef loadInstrLHS;
	string LHSVarName(varLHS->var.name);
	map<string, stack<LLVMValueRef> >::iterator found = CurrVarToAlloca.find(LHSVarName);
	if (found != CurrVarToAlloca.end()) {
		loadInstrLHS = CurrVarToAlloca[LHSVarName].top();
	}
	else {
		printf("\n ERROR: Assignment into unknown variable!!\n");
		exit(0);
	}
	// on the right hand side, we have an expression: either a BExpr or a UExpr
	// or a call to read!
	LLVMValueRef exprInstr;
	if (exprRHS->type == ast_bexpr) {
		printf("\nAssignment of binary expression!\n");
		exprInstr = handleBExpr (exprRHS, builder);
	} else if (exprRHS->type == ast_uexpr) {
		printf("\nAssignment of unary expression!\n");
		exprInstr = handleUExpr (exprRHS, builder);
	} else if (exprRHS->type == ast_stmt && exprRHS->stmt.type == ast_call) {
		printf("\nAssignment of read()\n");
		exprInstr = handleCall (exprRHS, builder);
	} else if (exprRHS->type == ast_cnst) {
		printf("\nAssignment of constant\n");
		exprInstr = handleConst (exprRHS, builder);
	} else if (exprRHS->type == ast_var) {
		printf("\nAssignment of variable.\n");
		exprInstr = handleVar (exprRHS, builder);
	}
	else {
		printf ("\n ERROR Expression of unknown type!\n");
		exit (0);
	}
	LLVMBuildStore (builder, exprInstr, loadInstrLHS);
}

/* Handles a declaration by adding the necessary alloca command and modifying the
 * stack of alloca commands corresponding to this variable name
 */
void handleDecl (astNode *declNode, LLVMBuilderRef& builder) {
	assert(declNode!= NULL && declNode->type == ast_stmt && declNode->stmt.type == ast_decl);
	assert(declNode->stmt.decl.name != NULL);

	printf("\nHandleDecl() is called!\n");
	printf("\nDeclaration variable name: %s\n", declNode->stmt.decl.name);
	string varName(declNode->stmt.decl.name);
	// position builder at alloca adding position (beginning of first basic block)
	LLVMPositionBuilderBefore (builder, allocaEndInstr);
	LLVMValueRef newAlloc = LLVMBuildAlloca(builder, LLVMInt32Type(), declNode->stmt.decl.name); 
	LLVMSetAlignment(newAlloc, 4);
	// re-position builder where it was
	LLVMPositionBuilderAtEnd (builder, currBasicBlock);
	cout << endl << "Variable name: " << varName << endl;
	printf("Alloca command: \n");
	LLVMDumpValue (newAlloc);
	CurrVarToAlloca [varName].push (newAlloc);
	printf("\n--------------------\n");
}

/* Handles a block of code by going through the list of statements 
 * and generating code for each of them
 */
void handleBlock (astNode *blockNode, LLVMBuilderRef& builder) {
	assert(blockNode != NULL && blockNode->type == ast_stmt && blockNode->stmt.type == ast_block);
	printf("handleBlock() is called!\n");
	// we keep track of all the new variables declared at the start of this
	// block
	// so that we can empty the alloca stacks at the end
	vector<string> declaredVars;
	for(vector<astNode *>::reverse_iterator rit = blockNode->stmt.block.stmt_list->rbegin();
											  rit != blockNode->stmt.block.stmt_list->rend();
											  ++rit) {
		switch ((*rit)->stmt.type) {
			case ast_call: 
				handleCall ((*rit), builder);
				break;
			case ast_ret: 
				handleRet ((*rit), builder);
				break;
			case ast_while:
				handleWhile ((*rit), builder);
				break;
			case ast_if: 
				handleIf ((*rit), builder);
				break;
			case ast_asgn:	
				handleAsgn ((*rit), builder);
				break;
			case ast_decl:
				// a new variable is declared! 
				// push it on the stack of variables declared
				// in this block!
				string newVarName((*rit)->stmt.decl.name);
				declaredVars.push_back(newVarName);
				handleDecl ((*rit), builder);
				break;
		}
	}
	for(vector<string>::iterator it = declaredVars.begin();
										  it != declaredVars.end();
										  ++it) {
		// pop this alloca command from the stack of alloca
		// commands corresponding to the variable with this name
		CurrVarToAlloca [(*it)].pop(); 
		// if the stack became empty, we now don't know this variable!
		// so delete the stack and the entry altogether
		if (CurrVarToAlloca [(*it)].empty()) {
			CurrVarToAlloca.erase ((*it));
		}
	}
}

void astToLLVMIR (astNode *root) {
	//Creating a module 
   LLVMModuleRef mod = LLVMModuleCreateWithName("");

	// TODO: Deal with the extern definitions by creating function declrations
	// Assume that extern1 -> read and extern2 -> print WLOG
	astNode *externR = root->prog.ext1; // extern int read(void);
	astNode *externP = root->prog.ext2; // extern void print(int);
	LLVMTypeRef param_types_read [] = {};
	LLVMTypeRef param_types_print [1] = { LLVMInt32Type() };
	readFunc = LLVMAddFunction(mod, readName, LLVMFunctionType( LLVMInt32Type(), param_types_read, 0, 0));
	printFunc = LLVMAddFunction(mod, printName, LLVMFunctionType (LLVMVoidType(), param_types_print, 1, 0));

	
	/* **************************** FUNCTION DEFINITION ******************************/
	// move to the function
	astNode *curr_node = root->prog.func;

	//Creating a function with a module
	LLVMTypeRef param_types [1];
	int num_of_args;
	if (curr_node->func.param == NULL) {
		// no parameters
		num_of_args = 0;
	}
	else {
		// in miniC we can have at most one integer parameter
		param_types[0] = LLVMInt32Type();
		num_of_args = 1;
	}
   LLVMTypeRef ret_type = LLVMFunctionType( LLVMInt32Type(), param_types, num_of_args, 0);
	char *funcName;
	if (curr_node->func.name == NULL) {
		funcName = "";
	} else {
		funcName = curr_node->func.name;
	}
   func = LLVMAddFunction(mod, funcName, ret_type);

	/* ********************************************************************************/

	// first, lets make our builder which we will use throughout the construction
	// of our IR:
	LLVMBuilderRef builder = LLVMCreateBuilder();

	//Creating our first basic block
   LLVMBasicBlockRef firstBB = LLVMAppendBasicBlock(func, "");
	// our current basic block is this first basic block
	currBasicBlock = firstBB;
	// position the builder at the end of this basic block
   LLVMPositionBuilderAtEnd(builder, firstBB);
	
	// build alloc instructions for the (possibly existing) argument
	// and create store instructions to make local copy of parameters.
	if (num_of_args == 1) {
		//Creating an alloc instruction with alignment
		LLVMValueRef arg = LLVMBuildAlloca(builder, LLVMInt32Type(), curr_node->func.param->var.name); 
		LLVMSetAlignment(arg, 4);
		LLVMBuildStore(builder, LLVMGetParam(func, 0), arg);

		string argName (curr_node->func.param->var.name);
		cout << endl << "Argument name: " << argName << endl;
		printf ("Corresponding alloca command: \n");
		LLVMDumpValue(arg);
		CurrVarToAlloca [argName].push(arg);
	}
	// mark where alloca instructions end
	allocaEndInstr = LLVMBuildAlloca (builder, LLVMInt32Type(), "allocaEndInstruction");

	// moving on to the body
	curr_node = curr_node ->func.body;

	// call the helper function that will build the IR and traverse the tree
	handleBlock (curr_node, builder);
	// delete the alloca instruction markers
	LLVMInstructionEraseFromParent (allocaEndInstr);

	/*********************************************************************************/

	//Writing out the LLVM IR
	LLVMDumpModule(mod);
	// print the IR into a file
	LLVMPrintModuleToFile (mod, "resultingIR.ll", NULL);

	//Cleanup
   LLVMDisposeBuilder(builder);
	LLVMDisposeModule(mod);
}
