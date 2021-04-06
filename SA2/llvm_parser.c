#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>

#define prt(x) if(x) { printf("%s\n", x); }

LLVMModuleRef createLLVMModel(char * filename){
	char *err = 0;

	LLVMMemoryBufferRef ll_f = 0;
	LLVMModuleRef m = 0;

	LLVMCreateMemoryBufferWithContentsOfFile(filename, &ll_f, &err);

	if (err != NULL) { 
		prt(err);
		return NULL;
	}
	
	LLVMParseIRInContext(LLVMGetGlobalContext(), ll_f, &m, &err);

	if (err != NULL) {
		prt(err);
	}

	return m;
}

void walkBBInstructions(LLVMBasicBlockRef bb){
	for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
  				instruction = LLVMGetNextInstruction(instruction)) {
	
		fprintf(stderr, "\nExploring uses of an instruction\n");
		fprintf(stderr, "Instruction: ");

		LLVMDumpValue (instruction);
		fprintf(stderr, "\n");

		// for every instruction, get its uses
		bool firstLoadWasDetected = false; // boolean indicator of whether we have seen the first load
		LLVMValueRef firstLoad; // we store the first load we saw
		// ASSUMPTION (which is probably true): LLVMGetNextUse() returns the uses of an instruction
		// in reverse order, i.e. the first use returned is the last one in the def-use chain.
		for (LLVMUseRef u = LLVMGetFirstUse(instruction); u; u = LLVMGetNextUse(u)) {
			LLVMValueRef x = LLVMGetUser(u);
			fprintf(stderr, "Use: ");
			LLVMDumpValue(x);
			fprintf(stderr, "\n");
			// if the use is a load instruction
			if (LLVMIsALoadInst (x)) {
				fprintf(stderr, "LOAD!\n");

				if (firstLoadWasDetected) {
					// get rid of redundant first load
					fprintf(stderr, "Removing redundant load:\n");
					LLVMDumpValue (firstLoad);

					printf("\n------------------------\n");
					LLVMInstructionRemoveFromParent (firstLoad);
					LLVMReplaceAllUsesWith (firstLoad, x);
					fprintf (stderr, "Redundant Load Removed\n");
					firstLoad = x;
				}
				else {
					fprintf(stderr, "First load is detected!\n");
					firstLoadWasDetected = true;
					firstLoad = x;
				}
			}
			else if (LLVMIsAStoreInst (x)) {
				// if a store instruction is detected, we have to start all over,
				// as the load instructions that came before may not now be redundant.
				fprintf(stderr, "STORE!\n");

				if (firstLoadWasDetected)
					firstLoadWasDetected = false;
			}
		}
	}

}


void walkBasicblocks(LLVMValueRef function){
	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
 			 basicBlock;
  			 basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		
		printf("In basic block\n");
		walkBBInstructions(basicBlock);
	
	}
}

void walkFunctions(LLVMModuleRef module){
	for (LLVMValueRef function =  LLVMGetFirstFunction(module); 
			function; 
			function = LLVMGetNextFunction(function)) {

		const char* funcName = LLVMGetValueName(function);	

		printf("Function Name: %s\n", funcName);

		walkBasicblocks(function);
 	}
}

void walkGlobalValues(LLVMModuleRef module){
	for (LLVMValueRef gVal =  LLVMGetFirstGlobal(module);
                        gVal;
                        gVal = LLVMGetNextGlobal(gVal)) {

                const char* gName = LLVMGetValueName(gVal);
                printf("Global variable name: %s\n", gName);
        }
}

int main(int argc, char** argv)
{
	LLVMModuleRef m;

	if (argc == 2){
		m = createLLVMModel(argv[1]);
	}
	else{
		m = NULL;
		return 1;
	}

	if (m != NULL){
		//LLVMDumpModule(m);
		walkFunctions(m);
		LLVMPrintModuleToFile (m, "testOpt.ll", NULL);
	}
	else {
	    printf("m is NULL\n");
	}
	
	return 0;
}
