#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cassert>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>

#include<vector>

using namespace std;

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

/* ************* LOCAL OPTIMIZATION FUNCTIONS *********************/

/* Checks if two instructions have the same operands
 * and the same op_code */
bool shouldEliminate (LLVMValueRef I, LLVMValueRef J) {
	int opI = LLVMGetNumOperands(I);
	int opJ = LLVMGetNumOperands(J);
	if (opI == opJ) {
		// works because of three variable form
		if (opI == 1) {
			if (LLVMGetOperand(I,0) == LLVMGetOperand(J,0)
					&& LLVMGetInstructionOpcode (I) == LLVMGetInstructionOpcode(J)) {
				return true;
			}
		}
		else if(opI == 2) {
			if (LLVMGetOperand(I,0) == LLVMGetOperand(J,0) &&
				 LLVMGetOperand(I,1) == LLVMGetOperand(J,1) &&
				 LLVMGetInstructionOpcode (I) == LLVMGetInstructionOpcode(J)) {
				 return true;
			}
		}
	}
	return false;
}

void eraseInstructions (vector<LLVMValueRef>& toDelete) {
	// delete all marked load instructions
	for (vector<LLVMValueRef>::iterator it = toDelete.begin();
													it != toDelete.end();
													++it) {
		LLVMInstructionEraseFromParent ((*it));
	}
}

/*
 * Common Subexpression Eliminiation Routine!
 * For a single basic block.
 */
void CSE(LLVMBasicBlockRef bb){
	// for each instruction I in the basic block
	printf("\nRunning Common Subexpression Elimination Routine\n");
	for (LLVMValueRef I = LLVMGetFirstInstruction(bb); I;
  				I = LLVMGetNextInstruction(I)) {
		printf("\n Examining command: \n");
		LLVMDumpValue(I);

		// if the instruction is an alloca instruction, move on
		if (LLVMGetInstructionOpcode(I) == LLVMAlloca) {
			printf ("\n Is alloca! Moving on! \n");
			continue;
		}

		// simple case: not a load instruction
		// so we don't have to check if the common subexpression reappears
		if (!LLVMIsALoadInst(I)) {
			printf("\n Not a load command!\n");
			// go through all the next instructions
			for (LLVMValueRef J = LLVMGetNextInstruction(I); J;
					J = LLVMGetNextInstruction(J)) {
				// and if one has a common subexpression with I,
				// have all uses of J with I
				if (shouldEliminate (I, J)) {
					printf("FOUND COMMON SUBEXPRESSION!!\n");
					LLVMDumpValue(J);
					LLVMReplaceAllUsesWith (J, I);
				}
			}
		}
		else {
			printf("\n A load command! Instructions follow: \n");
			// for load instructions an additional check must be performed
			for (LLVMValueRef J = LLVMGetNextInstruction(I); J;
					J = LLVMGetNextInstruction (J)) {
				LLVMDumpValue (J);
				printf("\n");
				// if we see a store instruction storing into what was loaded from I, we quit
				if (LLVMIsAStoreInst (J) && LLVMGetOperand(J, 1) == LLVMGetOperand(I, 0)) {
					break;
				}
				if (shouldEliminate (I,J)) {
					printf("\nFOUND COMMON SUBEXPRESSION!!\n");
					LLVMDumpValue(J);
					printf("\n");
					LLVMReplaceAllUsesWith (J,I);
				}
			}
		}
	}
	printf("\nEnd of Common Subexpression Elimination Routine\n");
	printf("---------------------------------------------------\n");
}

/*
 * Dead Code Elimination (DCE) Routine
 * for single basic block.
 */
void DCE (LLVMBasicBlockRef bb) {
	printf("\nRunning Dead Code Elimination Routine\n");
	// go through all instructions
	vector<LLVMValueRef> toDelete;
	for (LLVMValueRef I = LLVMGetFirstInstruction(bb); I;
  				I = LLVMGetNextInstruction(I)) {
		
		// if the instruction has SIDE EFFECTS, we don't omit it!
		// Instructions with possible side effects are:
		// 	* Terminator instructions
		// 	* Store instructions
		// 	* Call Instructions
		if (LLVMIsATerminatorInst(I) || LLVMIsAStoreInst (I) || LLVMGetInstructionOpcode(I) == LLVMCall) {
			printf("\n Ignoring Terminator, store and call instructions: \n");
			LLVMDumpValue(I);
			printf("\n");
			continue;
		}

		// if an instruction has no uses, remove it
		printf("\n Examining all uses of instruction:\n");
		LLVMDumpValue(I);
		int uses = 0;
		for (LLVMUseRef u = LLVMGetFirstUse(I); u; u = LLVMGetNextUse(u)) {
			uses++;
			printf("\nUse: \n");
			LLVMValueRef x = LLVMGetUser(u);
			LLVMDumpValue(x);
		}
		printf("\n Finished examining uses\n");
		if (uses == 0) {
			printf("\n Removing a dead instruction:\n");
			LLVMDumpValue(I);
			toDelete.push_back(I);
		}
	}
	eraseInstructions (toDelete);
	printf("\n End of Dead Code Eliminiation Routine\n");
	printf("-------------------------------------------\n");
}


/* Constant folding routine
 * for single basic block
 *
 * Returns true if any folding happens
 */
bool constantFolding (LLVMBasicBlockRef bb) {
	bool res = false;
	vector<LLVMValueRef> toDelete;
	printf("\nRunning Constant Folding Routine\n");
	// go through all instructions
	for (LLVMValueRef I = LLVMGetFirstInstruction(bb); I;
  				I = LLVMGetNextInstruction(I)) {
		// get the opcode for this instruction
		LLVMOpcode op = LLVMGetInstructionOpcode(I);
		// if it is a binary operator (+ - * /) AND
		// both operands are CONSTANTS,
		// perform the operation and replace all uses of the instruction
		// with the constant
		// Lastly, REMOVE that operation!
		if (op == LLVMAdd) {
			if (LLVMIsConstant(LLVMGetOperand(I, 0)) && 
					LLVMIsConstant(LLVMGetOperand(I, 1))) {
				printf ("\nTime to FOLD: \n");
				LLVMDumpValue (I);

				LLVMValueRef newConst = LLVMConstAdd (LLVMGetOperand(I,0), LLVMGetOperand(I,1));
				LLVMReplaceAllUsesWith (I, newConst);
				toDelete.push_back(I);
				res = true;
			}
		}
		else if (op == LLVMSub) {
			if (LLVMIsConstant(LLVMGetOperand(I, 0)) && 
					LLVMIsConstant(LLVMGetOperand(I, 1))) {
				printf ("\nTime to FOLD: \n");
				LLVMDumpValue (I);

				LLVMValueRef newConst = LLVMConstSub (LLVMGetOperand(I,0), LLVMGetOperand(I,1));
				LLVMReplaceAllUsesWith (I, newConst);
				toDelete.push_back(I);
				res = true;
			}
		}
		else if (op == LLVMMul) {
			if (LLVMIsConstant(LLVMGetOperand(I, 0)) && 
					LLVMIsConstant(LLVMGetOperand(I, 1))) {
				printf ("\nTime to FOLD: \n");
				LLVMDumpValue (I);

				LLVMValueRef newConst = LLVMConstMul (LLVMGetOperand(I,0), LLVMGetOperand(I,1));
				LLVMReplaceAllUsesWith (I, newConst);
				toDelete.push_back(I);
				res = true;
			}
		}
		else if (op == LLVMSDiv) {
			if (LLVMIsConstant(LLVMGetOperand(I, 0)) && 
					LLVMIsConstant(LLVMGetOperand(I, 1))) {
				printf ("\nTime to FOLD: \n");
				LLVMDumpValue (I);

				LLVMValueRef newConst = LLVMConstSDiv (LLVMGetOperand(I,0), LLVMGetOperand(I,1));
				LLVMReplaceAllUsesWith (I, newConst);
				toDelete.push_back(I);
				res = true;
			}
		}
	}
	eraseInstructions (toDelete);
	printf ("\n End of constant folding! \n");
	printf ("---------------------------------\n");
	return res;
}

/*	*************** End of local optimizations *************************/

/* ****************** Global optimizations ****************************/

/*********************** GEN *****************************************/
// returns true if victim is killed by killer store instruction
// ASSUMES both victim and killer are STORE instructions
bool isKilled (LLVMValueRef victim, LLVMValueRef killer) {
	// CAUTION: An instruction does not kill itself! This case will
	// not appear in GEN but will appear in KILL!
	
	if (!LLVMIsAStoreInst(victim) || !LLVMIsAStoreInst(killer)) {
		return false; // a precaution!
	}
	if (victim == killer)
		return false;
	return (LLVMGetOperand (victim, 1) == LLVMGetOperand (killer, 1));
}

/* Compute GEN[B] of a basic block B. Return a vector of
 * LLVMValueRef-s, pointing to the store instructions in 
 * GEN[B].
 */
vector<LLVMValueRef> GEN (LLVMBasicBlockRef bb) {
	vector<LLVMValueRef> gen;
	// for each instruction in the basic block
	for (LLVMValueRef I = LLVMGetFirstInstruction(bb); I;
  							I = LLVMGetNextInstruction(I)) {
		// if I is a store instruction, add it to GEN[B]
		if (LLVMIsAStoreInst(I))
			gen.push_back(I);
		// look through all the instructions in GEN[B] and remove
		// the ones killed by I
		for (vector<LLVMValueRef>::iterator it = gen.begin();
														it != gen.end(); ) {
			if (isKilled (*it, I))
				it = gen.erase(it);
			else
				++it;
		}	
	}
	return gen;
}

/* 
 * Computes the GEN of every basic block in a function, 
 * returning a list of GEN sets.
 */
vector< vector<LLVMValueRef> > computeGEN (LLVMValueRef function) {
	vector < vector<LLVMValueRef> > gens;
	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
 			 					  basicBlock;
  			 					  basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		gens.push_back(GEN(basicBlock));
	}
	return gens;
}

/* *************************************************************************************/

/* ***************************** KILL **************************************************/

/*
 * Computes KILL[B] for basic block bb, given the set S of 
 * all store instructions in this function.
 * Returns a vector of LLVMValueRef-s, pointing to 
 * the definitions killed by definitions in B
 */
vector<LLVMValueRef> KILL (LLVMBasicBlockRef bb, vector<LLVMValueRef> S) {
	vector<LLVMValueRef> kill;
	// for each store instruction in the function (pre-computed)
	for (vector<LLVMValueRef>::iterator it = S.begin();
												 	it != S.end();
													++it) {
		// if it is killed by something in this basic block, put it in kill
		// CAREFUL! An instruction cannot kill itself
		bool killed = false;
		for (LLVMValueRef I = LLVMGetFirstInstruction(bb); 
								I;
								I = LLVMGetNextInstruction(I)) {
			if (isKilled(*it, I)) {
				killed = true;
				break;
			}
		}

		if (killed) {
			kill.push_back (*it);
		}
	}
	return kill;
}

/* Computes the KILL of every basic block in a function and returns 
 * a list of KILL[B] for every basic block B in the 
 * Control Flow Graph
 */
vector< vector<LLVMValueRef> > computeKILL (LLVMValueRef function) {
	
	// First, we have to compute a list of all the store instructions in our function!
	vector<LLVMValueRef> S;
	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
								  basicBlock;
								  basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		for (LLVMValueRef I = LLVMGetFirstInstruction(basicBlock); I;
  								I = LLVMGetNextInstruction(I)) {
			if (LLVMIsAStoreInst (I)) {
				S.push_back(I);
			}
		}
	}

	// now we build the KILL[B] of each basic block B by using S
	vector < vector<LLVMValueRef> > kills;
	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
 			 					  basicBlock;
  			 					  basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		kills.push_back( KILL (basicBlock, S));
	}
	return kills;
}

/* *****************************************************************************/

/* *************************** IN & OUT ***************************************/

/*
 * Takes the union of two instruction sets
 */
vector<LLVMValueRef> Union (const vector<LLVMValueRef>& A, const vector<LLVMValueRef>& B) {
	vector<LLVMValueRef> res(A);
	for (vector<LLVMValueRef>::const_iterator itB = B.begin();
													itB != B.end();
													++itB) {
		bool addit = true;
		for (vector<LLVMValueRef>::const_iterator itA = A.begin();
														itA != A.end();
														++itA) {
			if ((*itB) == (*itA)) {
				addit = false;
				break;
			}
		}
		if (addit)
			res.push_back((*itB));
	}
	return res;
}

/*
 * Takes the difference of two instruction sets.
 * A - B
 */
vector<LLVMValueRef> Diff (const vector<LLVMValueRef>& A, const vector<LLVMValueRef>& B) {
	vector<LLVMValueRef> diff;
	for (vector<LLVMValueRef>::const_iterator itA = A.begin();
													itA != A.end();
													++itA) {
		bool addit = true;
		for (vector<LLVMValueRef>::const_iterator itB = B.begin();
														itB != B.end();
														++itB) {
			if ((*itB) == (*itA)) {
				addit = false;
				break;
			}
		}
		if (addit)
			diff.push_back(*itA);
	}
	return diff;
}

void computeINandOUTHelper (vector< vector<LLVMValueRef> >& ins,
							 		 vector< vector<LLVMValueRef> >& outs,
									 vector< vector<LLVMValueRef> >& gens,
									 vector< vector<LLVMValueRef> >& kills,
									 LLVMValueRef function) {
	printf("\nCompute IN and OUT for each basic block\n");
	bool change = true;
	while (change) {
		int i=0;
		// for all basic blocks, update IN[B] and OUT[B]
		int num = 0; // if there are no basic blocks, avoid infinite loop (comes up when using externs)
		for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
									  basicBlock;
									  basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
			num++;
			// IN[B] = Union(OUT[P1],OUT[P2],...,), where P1,P2,... are the predecessor
			// basic blocks of B
			//
			// LLVM's C API doesn't provide us a predecessor list function, so we make the
			// following work-around:
			//
			// Take each basic block as a possible predecessor and examine all its
			// possible successors. If one happens to be B, B is a predecessor!
			int j=0;
			for (LLVMBasicBlockRef possPred = LLVMGetFirstBasicBlock(function);
										  possPred;
										  possPred = LLVMGetNextBasicBlock(possPred)) {
				// get terminator instruction of possible predecessor	
				LLVMValueRef term = LLVMGetBasicBlockTerminator (possPred);
				// examine all the successors of that terminator
				for (int successor_id = 0; successor_id < LLVMGetNumSuccessors (term); successor_id++) {
					if (LLVMGetSuccessor (term, successor_id) == basicBlock) {
						ins[i] = Union( ins[i], outs[j] );
						break;
					}
				}
				j++;
			}

			// OUT[B] = GEN[B] UNION (IN[B] - KILL[B])
			vector<LLVMValueRef> oldout(outs[i]);
			outs[i] = Union (gens[i], Diff (ins[i], kills[i]));

			// compare the old and new OUT sets to know if we need to keep 
			// repeating our algorithm
			if (oldout.size() != outs[i].size())
				change = true;
			else {
				change = false;
				for (vector<LLVMValueRef>::iterator itOld = oldout.begin();
																itOld != oldout.end();
																++itOld) {
					bool isThere = false;
					for (vector<LLVMValueRef>::iterator itNew = outs[i].begin();
																	itNew != outs[i].end();
																	++itNew) {
						if ((*itOld) == (*itNew)) {
							isThere = true;
							break;
						}
					}
					if (!isThere) {
						change = true;
					}
				}
			}
			i++;
		}
		if (num == 0) {
			break;
		}
	}
}

/* ******************************************************************************/

/* ********************** CONSTANT PROPAGATION *********************************/

void verifyComputations (vector< vector<LLVMValueRef> >& computations, char *string) {
		for (vector< vector<LLVMValueRef> >::iterator it = computations.begin();
																	 it != computations.end();
																	 ++it) {
			printf("\n %s:\n", string);
			for (vector<LLVMValueRef>::iterator it2 = (*it).begin();
															it2 != (*it).end();
															++it2) {
		  		LLVMDumpValue(*it2);
				printf("\n");
			}
			printf ("\n--------------------\n");
		}
}

/*
 * Performs constant propagation on a given function
 * and returns if any changes were made or not!
 */
bool ConstantPropagation (LLVMValueRef function) {

	printf ("\n Calling Constant Propagation Function!\n");

	bool res = false;

	vector< vector<LLVMValueRef> > ins;
	vector< vector<LLVMValueRef> > outs;

	// initialize OUT[B] = GEN[B] for all basic blocks B
	vector< vector<LLVMValueRef> > gens = computeGEN(function);

	outs = vector< vector<LLVMValueRef> >(gens);

	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
								  basicBlock;
								  basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		// initialize IN[B] = emptyset for all basic blocks B
		ins.push_back ( vector<LLVMValueRef>() );
	}

	vector< vector<LLVMValueRef> > kills = computeKILL(function);

	printf ("\n Testing the computation of the KILL sets\n");
	verifyComputations (kills, "KILL[ BASIC BLOCK ]");
	printf ("\n Testing the computation of the GEN sets\n");
	verifyComputations (gens, "GEN[ BASIC BLOCK ]");
	
	// compute the IN and OUT sets of all basic blocks
	computeINandOUTHelper (ins, outs, gens, kills, function);

	printf ("\n Testing the computation of the IN sets\n");
	verifyComputations (ins, "IN[BASIC BLOCK]");
	printf ("\n Testing the computation of the OUT sets\n");
	verifyComputations (outs, "OUT[BASIC BLOCK]");

	// go through all basic blocks
	int k=0;
	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
								  basicBlock;
								  basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		int i=k; k++;
		// R is all the store instructions REACHING a given point
		vector<LLVMValueRef> R(ins[i]);
		// vector where we store instructions to delete
		vector<LLVMValueRef> toDelete;
		// go through all instructions in the basic block
		for (LLVMValueRef I = LLVMGetFirstInstruction(basicBlock); I;
  								I = LLVMGetNextInstruction(I)) {
			// update R according to new stores and kills being made
			if (LLVMIsAStoreInst (I)) {
				R.push_back(I);
				for (vector<LLVMValueRef>::iterator it = R.begin();
																it != R.end(); ) {
					if (isKilled (*it, I))
						it = R.erase(it);
					else
						++it;
				}
			}
			else if (LLVMIsALoadInst (I)) {
				// get what this instruction is loading from
				LLVMValueRef from = LLVMGetOperand(I, 0);
				// find all stores in R that write to %from and check if they are all constant stores
				// they also must store the SAME constant value!
				bool allConst = true, sameConst = true;
				// we store that value in variable ct
				int ct;
				bool ctInitialized = false; // initially, nothing is inside ct!
				// go over all reaching definitions that reach I
				for (vector<LLVMValueRef>::iterator it = R.begin();
																it != R.end();
																++it) {
					// sanity
					assert (LLVMIsAStoreInst ((*it)));
					// if the definition stores in %from
					if (LLVMGetOperand((*it), 1) == from) {
						// and if it stores a constant
						if ( ! LLVMIsConstant (LLVMGetOperand((*it),0)) ) {
							allConst = false;
							break;
						}
						else {
							// first time encountering a constant store into %from
							if ( !ctInitialized ) {
								ct = LLVMConstIntGetSExtValue ( LLVMGetOperand((*it), 0));
								ctInitialized = true;
							} 
							else {
								// every other time we encounter a constant store into %from,
								// we have to check if it stores the same constant!
								if (LLVMConstIntGetSExtValue (LLVMGetOperand((*it), 0)) != ct) {
									sameConst = false;
									break;
								}
							}
						}
					}
				}
				if (allConst && sameConst) {
					// replace the uses of I with the constant
					LLVMReplaceAllUsesWith (I, LLVMConstInt(LLVMInt32Type(), ct, true));
					// mark I as deleted
					toDelete.push_back(I);
					// a change has been made!
					res = true;
				}
			}
		}
		eraseInstructions (toDelete);
	}
	return res;
}

/* ********************************************************************************/

void walkFunctions(LLVMModuleRef module){
	bool changes;
	do {
		// repeat until convergence!
		for (LLVMValueRef function =  LLVMGetFirstFunction(module); 
								function; 
								function = LLVMGetNextFunction(function)) {

			// perform constant propagation and then constant folding 
			// on all basic blocks
			bool changesP = ConstantPropagation (function);	
			bool changesF = false;
			for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock (function);
										  bb;
										  bb = LLVMGetNextBasicBlock(bb)) {
				if (constantFolding(bb))
					changesF = true;
				// also perform Common Subexpression Elimination for all basic blocks 
				// and Dead Code Elimination
				CSE (bb);
				DCE (bb);
			}
			changes = changesP | changesF;
		}
	} while (changes);
}

/*void walkGlobalValues(LLVMModuleRef module){
	for (LLVMValueRef gVal =  LLVMGetFirstGlobal(module);
                        gVal;
                        gVal = LLVMGetNextGlobal(gVal)) {

                const char* gName = LLVMGetValueName(gVal);
                printf("Global variable name: %s\n", gName);
        }
}*/

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
