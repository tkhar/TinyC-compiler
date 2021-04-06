/*
 * Haris Themistoklis, Fall 2019
 *
 * COSC 57 - Compilers
 *
 * Register Allocation in Code generation
 *
 * Function overview
 */

#ifndef REGALLOC_H
#define REGALLOC_H

#include <cstdio>
#include <vector>
#include <set>
#include <map>
#include <stdlib.h>
#include <stdbool.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <utility>

using namespace std;

/*					Function declarations 			*/

/* 
 * Computes the register allocation map for a specific basic block.
 * This function will be called right after the live_range
 * mas has been computed for a specific basic block and makes direct
 * use of this data structure.
 *
 * NOTE: The number of registers we use is: 3 (%ebx, %ecx and %edx).
 * We reserve the %eax register as a spill register!
 *
 * Returns: the additional map created for this basic block
 */
map<LLVMValueRef, int> regAlloc (map<LLVMValueRef, pair<int, int> >& live_range);

#endif
