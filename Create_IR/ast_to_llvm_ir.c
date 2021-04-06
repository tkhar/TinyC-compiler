/*
 * Building the LLVM IR for the miniC language.
 *
 * Author: Haris Themistoklis
 *
 * Date: 10/31/19
 */
#include <stdio.h>
#include <llvm-c/Core.h>
#include <inttypes.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <stack>

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

/*
 * Handles a astVar node.
 * It searches in our map of current variable names <-> alloca instructions and
 * performs a load, which it then returns.
 *
 * CAUTION: It works the same of astDecl and astVar nodes, as they both contain 
 * a pointer to the variable name!
 */
LLVMValueRef handleVar (astNode *varNode, LLVMBuilderRef& builder) {
	// get an std::string version of the variable name
	string varName(varNode -> name);
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
	return LLVMConstInt (LLVMInt32Type(), constNode->value, 1);
}

/* 
 * Handles unary expressions.
 */
LLVMValueRef handleUExpr (astNode *uexpr, LLVMBuilderRef& builder) {
	// - (term)
	// term can be a variable or a constant
	// In both cases, we are getting the operand and making a unary
	// expression instruction
	// Afterwards, we return the resulting instruction.
	if (uexpr -> expr -> type == ast_var) {
		LLVMValueRef operand = handleVar (uexpr -> expr, builder);
		return LLVMBuildNeg (builder, operand);
	} 
	else if (uexpr -> expr -> type == ast_const) {
		LLVMValueRef operand = handleConst (uexpr -> expr, builder);
		return LLVMBuildNeg (builder, operand);
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
	// get the right hand and left hand sides of expression
	astNode *lhs = bexpr->lhs;
	LLVMValueRef lhsOperand;
	astNode *rhs = bexpr->rhs;
	LLVMValueRef rhsOperand;
	// get operator
	op_type op = bexpr->op;
	
	/* Depending on the type of operand, create different LLVM
	 * Instructions*/
	if (lhs -> type == ast_var) {
		lhsOperand = handleVar (lhs, builder);
	} 
	else if (lhs -> type == ast_const) {
		rhsOperand = handleConst (lhs, builder);
	}
	if (rhs -> type == ast_var) {
		rhsOperand = handleVar (rhs, builder);
	} 
	else if (rhs -> type == ast_const) {
		rhsOperand = handleConst (rhs, builder);
	}
	
	// depending on the operator type, generate a different LLVM Instruction
	switch (op) {
		case add: 
					 	return LLVMBuildAdd (builder, lhsOperand, rhsOperand);
					 
		case sub: 
					 	return LLVMBuildSub (builder, lhsOperand, rhsOperand);
					 
		case mul: 
						return LLVMBuildMul (builder, lhsOperand, rhsOperand);
					 
		case divide: 
						return LLVMBuildSDiv (builder, lhsOperand, rhsOperand);
						 
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
	// get the right hand and left hand sides of expression
	astNode *lhs = rexpr->lhs;
	LLVMValueRef lhsOperand;
	astNode *rhs = rexpr->rhs;
	LLVMValueRef rhsOperand;
	// get operator
	rop_type op = rexpr->op;
	
	/* Depending on the type of operand, create different LLVM
	 * Instructions.
	 * The operands could be variables, constants, unary expressions 
	 * binary expressions
	 */
	if (lhs -> type == ast_var) {
		lhsOperand = handleVar (lhs, builder);
	} 
	else if (lhs -> type == ast_const) {
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
	else if (rhs -> type == ast_const) {
		rhsOperand = handleConst (rhs, builder);
	}
	else if (lhs -> type == ast_uexpr) {
		rhsOperand = handleUExpr (rhs, builder);	
	}
	else if (lhs -> type == ast_bexpr) {
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
						return LLVMBuildICmp(builder, LLVMIntSEQ, lhsOperand, rhsOperand, "");
					
		case neq: 
						return LLVMBuildICmp(builder, LLVMIntSNE, lhsOperand, rhsOperand, "");
					 
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
	char *funcName = callNode -> name; // get function name
	if (strcmp(funcName, "read") == 0) {
		// for read, param is NULL, so just call the read function
		return LLVMBuildCall2(builder, LLVMInt32Type, readFunc, NULL, 0, "read");
	}
	else if (strcmp (funcName, "print") == 0) {
		// for print(), we have a param, which is a astDecl node!
		astNode *param = callNode -> param;
		LLVMValueRef par = handleVar (param, builder);
		return LLVMBuildCall2(builder, LLVMVoidType(), printFunc, par, 1, "print");
	}
}

/* 
 * Handles the return statement, which only contains a pointer
 * to an expression by making a return statement
 */
void handleRet (astNode *retNode, LLVMBuilderRef& builder) {
	if (retNode -> expr -> node_type == ast_bexpr) {
		LLVMBuildRet (builder, handleBExpr( retNode->expr, builder));
	}
	else if (retNode -> expr -> node_type == ast_uexpr) {
		LLVMBuildRet (builder, handleUExpr (retNode->expr, builder));
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
	// make a condition basic block
	LLVMBasicBlockRef cond_BB = LLVMAppendBasicBlock(func, "");
	LLVMPositionBuilderAtEnd (builder, cond_BB);
	// current basic block = cond_BB!
	currBasicBlock = cond_BB;
	// get the condition instruction
	LLVMValueRef condInstrBefore = handleRExpr (whileNode -> cond, builder); 
	//Creating basic blocks for while-body and for after while ends	
   LLVMBasicBlockRef while_BB = LLVMAppendBasicBlock(func, "");
   LLVMBasicBlockRef final = LLVMAppendBasicBlock(func, "");
	// conditionally branch
	// based on the condition instruction
	LLVMBuildCondBr(builder, condInstrBefore, while_BB, final);

	// go to the while basic block and build it
	LLVMPositionBuilderAtEnd(builder, while_BB);
	// the current basic block is the while block!
	currBasicBlock = while_BB;
	handleBlock (whileNode -> body);


	// head to the final basic block
	LLVMPositionBuilderAtEnd (builder, final);
	// update current basic block variable
	currBasicBlock = final;
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
	// get the condition instruction
	LLVMValueRef condInstr = handleRExpr (ifNode -> cond, builder); 

	//Creating basic blocks for if and for after if ends	
   LLVMBasicBlockRef if_BB = LLVMAppendBasicBlock(func, "");
   LLVMBasicBlockRef final = LLVMAppendBasicBlock(func, "");

	if (ifNode -> else_body == NULL) {
		// if no else exists, just go to final basic block in case
		// the condition is false
		LLVMBuildCondBr(builder, condInstr, if_BB, final);
		// go to the if basic block and build it
		LLVMPositionBuilderAtEnd(builder, if_BB);
		// the current basic block is the if block!
		currBasicBlock = if_BB;
		handleBlock (ifNode -> body);
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
		handleBlock (ifNode -> body);
		// put a branch to the final block at the end of the if block
		LLVMBuildBr (builder, final);
		// go to the else basic block and build it
		LLVMPositionBuilderAtEnd (builder, else_BB);
		// the current basic block is the else block!
		currBasicBlock = else_BB;
		handleBlock (ifNode -> else_body);
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
	// get left hand and right hand sides
	astNode *varLHS = asgnNode->lhs;
	astNode *exprRHS = astNode->rhs;
	// load the left hand side variable
	LLVMValueRef loadInstrLHS = handleVar(varLHS, builder);
	// on the right hand side, we have an expression: either a BExpr or a UExpr
	// or a call to read!
	LLVMValueRef exprInstr;
	if (exprRHS->node_type == ast_bexpr) {
		exprInstr = handleBExpr (exprRHS, builder);
	} else if (exprRHS->node_type == ast_uexpr) {
		exprInstr = handleUExpr (exprRHS, builder);
	} else if (exprRHS->node_type == ast_call) {
		exprInstr = handleCall (exprRHS, builder);
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
	string varName(declNode->name);
	// position builder at alloca adding position (beginning of first basic block)
	LLVMPositionBuilderBefore (builder, allocaEndInstr);
	LLVMValueRef newAlloc = LLVMBuildAlloca(builder, LLVMInt32Type(), declNode->name); 
	// re-position builder where it was
	LLVMPositionBuilderAtEnd (builder, currBasicBlock);
	CurrVarToAlloca [varName].push (newAlloc);
}

/* Handles a block of code by going through the list of statements 
 * and generating code for each of them
 */
void handleBlock (astNode *blockNode, LLVMBuilderRef& builder) {
	// we keep track of all the new variables declared at the start of this
	// block
	// so that we can empty the alloca stacks at the end
	vector<string> declaredVars;
	for(vector<astNode *>::iterator it = blockNode->stmt_list->begin();
											  it != blockNode->stmt_list->end();
											  ++it) {
		switch ((*it)->stmt.type) {
			case ast_call: 
				handleCall ((*it), builder);
				break;
			case ast_ret: 
				handleRet ((*it), builder);
				break;
			case ast_while:
				handleWhile ((*it), builder);
				break;
			case ast_if: 
				handleIf ((*it), builder);
				break;
			case ast_asgn:	
				handleAsgn ((*it), builder);
				break;
			case ast_decl:
				// a new variable is declared! 
				// push it on the stack of variables declared
				// in this block!
				string newVarName((*it)->name);
				declaredVars.push_back(newVarName);
				handleDecl ((*it), builder);
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
	astNode *externR = root->ext1; // extern int read(void);
	astNode *externP = root->ext2; // extern void print(int);
	LLVMTypeRef param_types_read = {};
	LLVMTypeRef param_types_print = { LLVMInt32Type() };
	readFunc = LLVMAddFunction(mod, "read", LLVMFunctionType( LLVMInt32Type(), param_types_read, 0, 0));
	printFunc = LLVMAddFunction(mod, "print", LLVMFunctionType (LLVMVoidType(), param_types_print, 1, 0));

	
	/* **************************** FUNCTION DEFINITION ******************************/
	// move to the function
	astNode *curr_node = root->func;

	//Creating a function with a module
	LLVMTypeRef param_types [];
	int num_of_args;
	if (curr_node->param == NULL) {
		// no parameters
		param_types = {};
		num_of_args = 0;
	}
	else {
		// in miniC we can have at most one integer parameter
		param_types = { LLVMInt32Type() };
		num_of_args = 1;
	}
   LLVMTypeRef ret_type = LLVMFunctionType( LLVMInt32Type(), param_types, num_of_args, 0);
	char *funcName;
	if (curr_node -> name == NULL) {
		funcName = ""
	} else {
		funcName = curr_node->name;
	}
   LLVMValueRef func = LLVMAddFunction(mod, funcName, ret_type);

	/* ********************************************************************************/

	// first, lets make our builder which we will use throughout the construction
	// of our IR:
	LLVMBuilderRef builder = LLVMCreateBuilder();

	//Creating our first basic block
   LLVMBasicBlockRef firstBB = LLVMAppendBasicBlock(func, "");
	// our current basic block is this first basic block
	currBasicBlock = firstBB;
	// position the builder at the end of this basic block
   LLVMPositionBuilderAtEnd(builder, first);
	
	// build alloc instructions for the (possibly existing) argument
	// and create store instructions to make local copy of parameters.
	if (num_of_args == 1) {
		//Creating an alloc instruction with alignment
		LLVMValueRef arg = LLVMBuildAlloca(builder, LLVMInt32Type(), curr_node->param->name); 
		LLVMSetAlignment(arg, 4);
		LLVMBuildStore(builder, LLVMGetParam(func, 0), arg);
	}
	// mark where alloca instructions end
	allocaEndInstr = LLVMBuildAlloca (builder, LLVMInt32Type(), "allocaEndInstruction");

	// moving on to the body
	curr_node = curr_node -> body;

	// call the helper function that will build the IR and traverse the tree
	createIRHelper (curr_node, builder, 1);
	// delete the alloca instruction markers
	LLVMInstructionEraseFromParent (allocaEndInstr);

	/*********************************************************************************/

	//Writing out the LLVM IR
	LLVMDumpModule(mod);

	//Cleanup
   LLVMDisposeBuilder(builder);
	LLVMDisposeModule(mod);
}
