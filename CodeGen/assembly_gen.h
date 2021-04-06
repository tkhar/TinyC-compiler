/*
 *
 * Haris Themistoklis, CS57 - Compilers, Nov. 2019
 * 
 * Header file for assembly code generation routines
 *
 */

#ifndef ASSEMBLYGEN_H
#define ASSEMBLYGEN_H

#include<cstdio>
#include<vector>
#include<map>
#include<set>
#include<utility>
#include<llvm-c/Core.h>
#include<llvm-c/IRReader.h>
#include<string>
#include<cstring>

using namespace std;

/*
 * Generates the assembly code for a function by using the instruction offset map
 * and the allocated registers. Writes the resulting code in a file
 * called "outputAssembly.s"
 */
void assembly_code_generator (LLVMValueRef func, map<LLVMValueRef, int>& offset_map, map<LLVMValueRef, int>& allocatedRegisters,
										map<LLVMBasicBlockRef, char *>& bb_labels, int localMem);

#endif
