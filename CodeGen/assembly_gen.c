/*
 *
 * Haris Themistoklis, CS57 - Compilers, November 2019
 *
 * Assembly code generation from LLVM IR.
 */

#include "assembly_gen.h"

/* --- Local Utility Functions --- */
char reg (int r) {
	if (r == 2)
		return 'b';
	if (r == 3)
		return 'c';
	if (r == 4)
		return 'd';
	return '!';
}

/***********************************/

/*				Main Function 				*/
// generates the assembly code for a given function
void assembly_code_generator (LLVMValueRef func, map<LLVMValueRef, int>& offset_map, map<LLVMValueRef, int>& allocatedRegisters,
											map<LLVMBasicBlockRef, char*>& bb_labels, int localMem) {
	printf("\nGenerating assembly code for function! \n");
	printf("\nLocalMem: %d\n", localMem); 

	// file to contain our assembly
	FILE *outputFile = fopen("outputAssembly.s","w");

	// STEP 1: Initialization directives!
	const char* funcName = LLVMGetValueName(func);
	fprintf(outputFile, "\t.globl %s\n", funcName);
	fprintf(outputFile, "\t.type %s, @function\n", funcName);
	fprintf(outputFile, "%s:\n",funcName);
	fprintf(outputFile, "\tpushl %%ebp\n");
	fprintf(outputFile, "\tmovl %%esp, %%ebp\n");
	fprintf(outputFile, "\taddl $%d, %%esp\n", localMem); // reserving space!
	fprintf(outputFile, "\tjmp %s\n", bb_labels [LLVMGetFirstBasicBlock (func)]);
	
	for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock (func);
								  bb;
								  bb = LLVMGetNextBasicBlock (bb)) {
		fprintf(outputFile, "%s:\n", bb_labels [bb]); 
		// loop through all basic blocks
		for (LLVMValueRef instr = LLVMGetFirstInstruction (bb);
								instr;
								instr = LLVMGetNextInstruction (instr)) {
			// for all instructions in a basic block
			if (LLVMIsAAllocaInst (instr)) {
				// ignore the alloca instructions 
				continue;
			}
			else if (LLVMIsAReturnInst (instr)) {
				// we have to leave the return value in %eax
				LLVMValueRef ret = LLVMGetOperand (instr, 0);
				if (LLVMIsConstant (ret)) {
					fprintf(outputFile, "\tmovl $%d, %%eax\n", LLVMConstIntGetSExtValue (ret));
				} else if (allocatedRegisters [ret] != -1) {
					if (allocatedRegisters [ret] == 0) {
						fprintf(stderr, "\n Error: ");
						LLVMDumpValue(ret);
						fprintf(stderr, " has allocated register of 0!\n");
					}
					fprintf(outputFile, "\tmovl %%e%cx, %%eax\n", reg (allocatedRegisters [ret]));
				} else {
					int offset = offset_map [ret];
					fprintf(outputFile, "\tmovl %d(%%ebp), %%eax\n", offset);
				}
				// FINAL STEP: Undoing initialization directives
				fprintf(outputFile, "\tmovl %%ebp, %%esp\n");
				fprintf(outputFile, "\tpopl %%ebp\n");
				fprintf(outputFile, "\tret\n");
			}
			else if (LLVMIsALoadInst (instr)) {
				// if our instruction is a load instruction,
				// then we only care about what happens when 
				// we are loading into something for which we 
				// have allocated a physical register
				// In that case, we just have to change the value 
				// that is in that register
				LLVMValueRef op = LLVMGetOperand(instr, 0);
				if (allocatedRegisters [instr] != -1) {
					int offset = offset_map [op];
					if (allocatedRegisters [instr] == 0) {
						fprintf(stderr, "\n Error: ");
						LLVMDumpValue(instr);
						fprintf(stderr, " has allocated register of 0!\n");
					}
					fprintf(outputFile, "\tmovl %d(%%ebp), %%e%cx\n", offset, reg (allocatedRegisters [instr]));
				}
			}
			else if (LLVMIsAStoreInst (instr)) {
				// a store instruction like :
				// store i32 op1, i32* %op2
				// Our job is to go to the place in that stack
				// where op2 lives and update the value
				LLVMValueRef op1 = LLVMGetOperand(instr, 0);
				LLVMValueRef op2 = LLVMGetOperand(instr, 1);
				if (LLVMIsAArgument (op1)) {
					// we already know that op2 lives in offset 8
					// and that only changes when the argument is
					// updated possibly.
					// No action is therefore necessary at this point
					continue;
				}
				else if (LLVMIsConstant (op1)) {
					int offset = offset_map [op2];
					fprintf(outputFile, "\tmovl $%d, %d(%%ebp)\n", LLVMConstIntGetSExtValue(op1),offset);
				}
				else {
					if (allocatedRegisters[op1] != -1) {
						// if op1 has a physical register
						int offset = offset_map [op2];
						if (allocatedRegisters [op1] == 0) {
							fprintf(stderr, "\n Error: ");
							LLVMDumpValue(op1);
							fprintf(stderr, " has allocated register of 0!\n");
						}
						fprintf(outputFile, "\tmovl %%e%cx, %d(%%ebp)\n", reg (allocatedRegisters[op1]), offset);
					}
					else {
						int offset1 = offset_map [op1];
						int offset2 = offset_map [op2];
						// move op1 to spill register %eax
						fprintf(outputFile, "\tmovl %d(%%ebp), %%eax", offset1);
						// move spill register to op2
						fprintf(outputFile, "\tmovl %%eax, %d(%%ebp)\n", offset2);
					}
				}
			}
			else if (LLVMIsACallInst (instr)) {
				// Possible calls:
				// 	1. %a = call A @read()
				// 	2. call A @print(Param)
				// when it is time to call another function,
				// first save our registers in the stack so as
				// to find them again when we return to this function
				fprintf(outputFile, "\tpushl %%ebx\n\tpushl %%ecx\n\tpushl %%edx\n");
				LLVMValueRef function = LLVMGetCalledValue (instr);
				if (LLVMGetNumArgOperands (instr) == 1) {
					printf ("\nCalling print()!\n");
					// print function
					// we must pushl on the stack the parameter
					LLVMValueRef param = LLVMGetArgOperand (instr, 0);
					fprintf(stderr, "\nParameter: ");
					LLVMDumpValue (param);
					fprintf(stderr, "\n");
					if (LLVMIsConstant(param)) {
						fprintf(outputFile, "\tpushl $%d\n", LLVMConstIntGetSExtValue (param));
					}
					else {
						if (allocatedRegisters [param] != -1) {
							if (allocatedRegisters [param] == 0) {
								fprintf(stderr, "\n Error: ");
								LLVMDumpValue(param);
								fprintf(stderr, " has allocated register of 0!\n");
							}
							fprintf(outputFile, "\tpushl %%e%cx\n", reg(allocatedRegisters [param]));
						}
						else {
							int offset = offset_map [param];
							fprintf(outputFile, "\tpushl %d(%%ebp)\n", offset);
						}
					}
					printf("\nPushed parameter \n");
				}
				// call the function
				fprintf(outputFile, "\tcall %s\n", LLVMGetValueName (function));
				// once function finishes, undo pushling of parameter, if one exists
				if (LLVMGetNumArgOperands (instr) == 1) {
					fprintf(outputFile, "\taddl $4, %%esp\n");
					// popl registers back into place
					fprintf(outputFile, "\tpopl %%edx\n\tpopl %%ecx\n\tpopl %%ebx\n");
				}
				else {
					// no parameters --> %a = read()
					// return value will be in %eax
					// We have to move it to where it belongs!
					// That happens AFTER we pop registers back into place!
					fprintf(outputFile, "\tpopl %%edx\n\tpopl %%ecx\n\tpopl %%ebx\n");
					if (allocatedRegisters [instr] != -1) {
						if (allocatedRegisters [instr] == 0) {
							fprintf(stderr, "\n Error: ");
							LLVMDumpValue(instr);
							fprintf(stderr, " has allocated register of 0!\n");
						}
						fprintf(outputFile, "\tmovl %%eax, %%e%cx\n", reg (allocatedRegisters[instr]));	
					}
					else {
						int offset = offset_map [instr];
						fprintf(outputFile, "\tmovl %%eax, %d(%%ebp)\n", offset);
					}
				}
			}
			else if(LLVMIsABranchInst (instr)) {
				if (LLVMIsConditional (instr)) {
					// conditional branch: br i1 %a, label %b, label %c
					fprintf(stderr, "\nConditional branch: ");
					LLVMDumpValue (instr);
					fprintf(stderr, "\nBasic blocks: ");
					// hm... the basic blocks are actually given in reverse order!
					// %b is 2 and %c is 1!
					LLVMDumpValue (LLVMGetOperand (instr, 1));
					fprintf(stderr, "\n");
					LLVMDumpValue (LLVMGetOperand (instr, 2));
					fprintf(stderr, "\nLabels: ");
					char *label2 = bb_labels [LLVMValueAsBasicBlock(LLVMGetOperand(instr, 1))];
					char *label1 = bb_labels [LLVMValueAsBasicBlock(LLVMGetOperand(instr, 2))];
					fprintf(stderr, "%s %s\n", label1, label2);
					LLVMValueRef cond = LLVMGetCondition (instr);
					LLVMIntPredicate pred = LLVMGetICmpPredicate (cond);
					if (pred == LLVMIntEQ) {
						fprintf(outputFile, "\tje %s\n", label1);
						fprintf(outputFile, "\tjmp %s\n", label2);
					}
					else if (pred == LLVMIntSLT){
						fprintf(outputFile, "\tjl %s\n", label1);
						fprintf(outputFile, "\tjmp %s\n", label2);
					}
					else if (pred == LLVMIntSLE){
						fprintf(outputFile, "\tjle %s\n", label1);
						fprintf(outputFile, "\tjmp %s\n", label2);
					}
					else if (pred == LLVMIntSGT) {
						fprintf(outputFile, "\tjg %s\n", label1);
						fprintf(outputFile, "\tjmp %s\n", label2);
					}
					else if (pred == LLVMIntSGE) {
						fprintf(outputFile, "\tjge %s\n", label1);
						fprintf(outputFile, "\tjmp %s\n", label2);
					}
					else if (pred == LLVMIntNE) {
						fprintf(outputFile, "\tjne %s\n", label1);
						fprintf(outputFile, "\tjmp %s\n", label2);
					}
				}
				else {
					// not conditional branch: br label %b
					char *label = bb_labels [LLVMValueAsBasicBlock(LLVMGetOperand(instr, 0))];
					fprintf (outputFile, "\tjmp %s\n",label);
				}
			}
			else {
				// the operation will be an arithmetic operation!
				// add, sub, mul, icmp
				//
				// There a small complication with division
				// idivl %ebx will do (%eax) / (%ebx)
				// and put the remainder in %edx! What to do here?
				
				// First, where will the result live?
				char *livingLocation = (char *)malloc(10*sizeof(char));
				if (allocatedRegisters [instr] != -1) {
					// register
					if (allocatedRegisters [instr] == 0) {
						fprintf(stderr, "\n Error: ");
						LLVMDumpValue(instr);
						fprintf(stderr, " has an allocated register of 0!\n");
					}
					sprintf(livingLocation, "%%e%cx", reg (allocatedRegisters [instr]));
				}
				else {
					// memory
					strcpy(livingLocation, "%%eax");
				}
				// get operator and operands (there will always be 2 operands)
				LLVMOpcode op = LLVMGetInstructionOpcode (instr);
				LLVMValueRef op1 = LLVMGetOperand (instr, 0);
				LLVMValueRef op2 = LLVMGetOperand (instr, 1);
				// load the first operand. Start with this because in subtraction the order matters!
				if (LLVMIsConstant (op1)) {
					fprintf(outputFile, "\tmovl $%d, %s\n", LLVMConstIntGetSExtValue (op1), livingLocation);
				}
				else if(allocatedRegisters [op1] != -1){
					if (allocatedRegisters [op1] == 0) {
						fprintf(stderr, "\n Error: ");
						LLVMDumpValue(op2);
						fprintf(stderr, " has an allocated register of 0!\n");
					}
					fprintf(outputFile, "\tmovl %%e%cx, %s\n", reg (allocatedRegisters [op1]), livingLocation);
				}
				else{
					int offset = offset_map [op1];
					fprintf(outputFile, "\tmovl %d(%%ebp), %s\n", offset, livingLocation);
				}
				// figure out what operation is being performed
				char *operation;
				if (op == LLVMICmp) {
					operation = "cmp"; 
				}
				else if (op == LLVMAdd) {
					operation = "addl";
				}
				else if (op == LLVMSub) {
					operation = "subl";
				}
				else if (op == LLVMMul) {
					operation = "imull";
				}
				// perform the operation by loading the second operand
				if (LLVMIsConstant (op2)) {
					fprintf(outputFile, "\t%s $%d, %s\n", operation, LLVMConstIntGetSExtValue (op2), livingLocation);
				}
				else {
					if (allocatedRegisters [op2] != -1) {
						if (allocatedRegisters [op2] == 0) {
							fprintf(stderr, "\n Error: ");
							LLVMDumpValue(op2);
							fprintf(stderr, " has an allocated register of 0!\n");
						}
						fprintf(outputFile, "\t%s %%e%cx, %s\n", operation, reg(allocatedRegisters [op2]), livingLocation);	
					}
					else {
						int offset = offset_map [op2];
						fprintf(outputFile, "\t%s %d(%%ebp), %s\n", operation, offset, livingLocation);
					}
				}
				if (allocatedRegisters [instr] == -1) {
					// copy the result from %eax to memory
					int offset = offset_map [instr];
					fprintf(outputFile, "\tmovl %%eax, %d(%%ebp)", offset);
				}
			}
		}
	}

	fclose(outputFile);

	printf("\n -------------------------------------- \n");
}
/* *********************************** */

