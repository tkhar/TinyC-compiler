#include "codegen.h"
#include "reg_alloc.h"
#include "assembly_gen.h"

int num_digits(int n){ 
    int j = 0;
    while (n >= 1){ 
        n = n / 10; 
        j = j + 1;
    }   
    return j;
}

LLVMModuleRef createLLVMModel(char * filename){
	char *err = 0;

	LLVMMemoryBufferRef ll_f = 0;
	LLVMModuleRef m = 0;

	LLVMCreateMemoryBufferWithContentsOfFile(filename, &ll_f, &err);

	if (err != NULL) { 
		return NULL;
	}
	
	LLVMParseIRInContext(LLVMGetGlobalContext(), ll_f, &m, &err);
	return m;
}

void printDirectives(LLVMValueRef func){
	printf("\n --- Initialization directives ----\n\n");
	const char* funcName = LLVMGetValueName(func);	
	printf("\t.globl %s\n", funcName);
	printf("\t.type %s, @function\n", funcName);
	printf("%s:\n",funcName);
	printf("\tpushl %%ebp\n");
	printf("\tmovl %%esp, %%ebp\n");
	printf("\n--------------------------------\n");
	
}

void printFunctionEnd(){
	printf("\n --- Termination directives ----------\n");
	printf("\tmovl %%ebp, %%esp\n");
	printf("\tpopl %%ebp\n");
	printf("\tret\n");
	printf("\n------------------------------------\n");
}

//Go over all the basic blocks and create the labels
void createBBLabels(LLVMValueRef function){
	int i = 0;
	const char * base = ".B";

	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
 			 basicBlock;
  			 basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

			char * s = (char *) calloc(num_digits(i) + 3, sizeof(char));
			sprintf(s, "%d", i);
			char * label = (char *) calloc(strlen(base)+strlen(s)+1, sizeof(char));
			strncpy(label, base, 2);
			strcat(label, s);
			
			bb_labels.insert(pair<LLVMBasicBlockRef, char *> (basicBlock, label));
			free(s);
			i = i + 1;
			
	}
	
}

//Populates the map from LLVMValueref (instruction) to index
void create_inst_index(LLVMBasicBlockRef bb){
		int n = 0;
        for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
                                instruction = LLVMGetNextInstruction(instruction)) {
			if (!LLVMIsAAllocaInst(instruction)){
				inst_index.insert(pair<LLVMValueRef, int> (instruction, n));
				n = n + 1;
			}
		}	
}

// Computes the liveness range of each instruction. start is the index of the def and end
// is the index of last use in this basic block.

void compute_liveness(LLVMBasicBlockRef bb){
		
		// Compute the index of all the instructions
		create_inst_index(bb);

        for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
                                instruction = LLVMGetNextInstruction(instruction)) {
			
			// Ignore the alloc instructions
			if (LLVMIsAAllocaInst(instruction)) continue;

			int start, end;
			
			start = inst_index[instruction];
			end = start;
			
			// Compute the last use of the instruction in this basic block
		 
			for (LLVMUseRef u = LLVMGetFirstUse(instruction); u;
					u = LLVMGetNextUse(u)){
				LLVMValueRef user = LLVMGetUser(u);
				
				// Check if use is in the same basic block (this should be the case)
				if (LLVMGetInstructionParent(instruction) == LLVMGetInstructionParent(user)){
					int index = inst_index[user];	
					if (index > end)
							end = index;
				}
				
			}
			
			pair<int, int> range (start, end);
			live_range.insert(pair<LLVMValueRef, pair<int, int>> (instruction, range));
		}
}

void clear_data(){
	inst_index.clear();
	live_range.clear();
	return;
}

void getOffsetMap(LLVMValueRef func){
	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(func);
 			 basicBlock;
  			 basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		
		// Loop to add all the instructions into offset_map with an offset 0
        for (LLVMValueRef instr = LLVMGetFirstInstruction(basicBlock); instr;
                                instr = LLVMGetNextInstruction(instr)) {
			// Ignore if it is not generating a value	
			if (LLVMIsATerminatorInst(instr) || LLVMIsAStoreInst(instr))
				continue;
			
			// Ignore a call instruction if it doesn't have any use
			if (LLVMIsACallInst(instr))
				if (LLVMGetFirstUse(instr) == NULL)
					continue;
			
			
			offset_map.insert(pair<LLVMValueRef, int> (instr, 0));
		}
		
		//Loop over instructions to update offset for temporary variables
		//holding addresses of local variables and temporary variables
		//corresponding to local variables.
        for (LLVMValueRef instr = LLVMGetFirstInstruction(basicBlock); instr;
                                instr = LLVMGetNextInstruction(instr)) {
				fprintf(stderr, "\nInstruction examined: ");
				LLVMDumpValue (instr);
				fprintf(stderr, "\n");
				if (LLVMIsAStoreInst(instr)){
					fprintf(stderr, "A store instruction: ");
					LLVMValueRef op1 = LLVMGetOperand(instr, 0);
					LLVMValueRef op2 = LLVMGetOperand(instr, 1);

					// If store is from a parameter to the local memory
					// set both the offsets to 8. In miniC we have 
					// maximum of 1 parameter.
					
					if (LLVMIsAArgument(op1)) {
						fprintf(stderr, "storing argument!\n");
						offset_map[op1] = 8;
						offset_map[op2] = 8;
					}	
					else {
						// If the offset of the address variable is not
						// set then set it localMem and update localMem
						if (offset_map[op2] == 0){
							fprintf(stderr, "updating local mem: %d", localMem);
							localMem = localMem - 4;
							offset_map[op2] = localMem;
						}
						
						// Link temporary variable in op1 to local variable
						if (! LLVMIsAConstantInt(op1)){
							fprintf(stderr, " moving to location: %d\n",offset_map[op2]);
							offset_map[op1] = offset_map[op2];
						}
					}
				}

				if (LLVMIsALoadInst(instr)){
					LLVMValueRef op1 = LLVMGetOperand(instr, 0);

					if (offset_map[op1] == 0){
						fprintf(stderr, "Updating local mem: %d\n", localMem);
						localMem = localMem - 4;
						offset_map[op1] = localMem;
					}
					
					//Link this instruction to the memory address of local
					//variable.
					fprintf(stderr, "offset of instruction: %d\n", offset_map[op1]);
					offset_map[instr] = offset_map[op1];
				}
		}

		//Loop over all instructions(values) in the offset_map and find 
		//those with offset 0. These temporary variables do not 
		//correspond to any local variable in the miniC code. 
		map<LLVMValueRef, int>::iterator it;
		for (it = offset_map.begin(); it != offset_map.end(); it++){
			if (it->second == 0) {
				localMem = localMem - 4;
				it->second = localMem;
			}
		}
		
		printf("\n--- Offset map calculations ---\n");
		map<LLVMValueRef, int>::iterator it1;
		printf("\n Local Memory size: %d\n", localMem);
		for (it1 = offset_map.begin(); it1 != offset_map.end(); it1++){
			LLVMDumpValue(it1->first);
			printf(":\t %d\n", it1->second);
		}
		printf("\n ------------------------------\n");
		
	}
	
}

void walkBasicblocks(LLVMValueRef function){
	//Go over all the basic blocks and create the labels
	
	printf("\n --- Computing register allocation --- \n\n");

	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
 			 basicBlock;
  			 basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		compute_liveness(basicBlock);
		
		// build the allocated registers map one basic block at a time
		// by using the linear scan register allocation algorithm
		map<LLVMValueRef, int> basicBlockRegAllocs = regAlloc (live_range);
		// paste this new piece of our map into the end result
		printf("\n --- New Basic Block --- \n");
		for (map<LLVMValueRef, int>::iterator it = basicBlockRegAllocs.begin();
														  it != basicBlockRegAllocs.end();
														  ++ it) {
			LLVMDumpValue(it->first);
			printf(": Interval: [%d %d]\n", live_range[it->first].first, live_range[it->first].second);
			allocatedRegisters [it->first] = it->second;
		}
		printf("\n -------------------------- \n");
		clear_data();
	}
	printf("\n ------------ Done ---------------------\n");
	printf("\n --- Allocated Registers ---\n\n");
	for(map<LLVMValueRef, int>::iterator it = allocatedRegisters.begin();
													 it != allocatedRegisters.end();
													 ++it) {
		LLVMDumpValue(it->first);
		printf(" --> Register: %d\n", it->second);
	}
	printf("\n -----------------------------\n");
}

void walkFunctions(LLVMModuleRef module){
	for (LLVMValueRef function =  LLVMGetFirstFunction(module); 
			function; 
			function = LLVMGetNextFunction(function)) {
		
		if (!LLVMIsDeclaration(function)){
			// initialization assembly directives for function
			printDirectives(function);
			createBBLabels(function);
			// walk through all basic blocks and allocate registers
			// ASSUME registers are free at the beginning of basic block.
			// This is guaranteed by LLVM IR Code structure and by the 
			// fact that we didn't really erase any store instructions earlier
			walkBasicblocks(function);
			// fill the offset map for this function
			getOffsetMap(function);	
			printFunctionEnd();
			// generate the assembly code for this function
			assembly_code_generator (function, offset_map, allocatedRegisters, bb_labels, localMem);
		}
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
		walkFunctions(m);
		//LLVMDumpModule(m);
	}
	else {
	    printf("m is NULL\n");
	}
	
	return 0;
}
