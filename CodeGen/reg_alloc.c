/*
 * Haris Themistoklis, Fall 2019
 *
 * COSC 57 - Compilers
 *
 * Register Allocation in Code generation.
 *
 * Here we implement the linear scan register allocation algorithm,
 * taken from the paper 
 * http://web.cs.ucla.edu/~palsberg/course/cs132/linearscan.pdf
 */

#include "reg_alloc.h"

/* Local function and type declarations; global variables */

typedef pair<int,int> ii;

struct sortByStart {
	bool operator() (const ii& a, const ii& b) const {
		if (a.first != b.first) 
			return a.first < b.first;
		return a.second < b.second;
	}
};

struct sortByEnd {
	bool operator() (const ii& a, const ii& b) const {
		if(a.second != b.second)
			return a.second < b.second;
		return a.first < b.first;
	}
};

bool regB, regC, regD;

// removes dead intervals from active set and updates the pool of free registers
void ExpireOldIntervals(ii, set<ii, sortByEnd>&, map<LLVMValueRef, int>&, map<ii, LLVMValueRef, sortByStart>& );
// spils the latets interval between i itself and the active set
void SpillAtInterval(ii, set<ii, sortByEnd>& , map<LLVMValueRef, int>&, map<ii, LLVMValueRef, sortByStart>& );

/* ************************************ */


/* Local function definitions */
void ExpireOldIntervals( ii i, set<ii, sortByEnd>& active, 
								 map<LLVMValueRef, int>& regist, map<ii, LLVMValueRef, sortByStart>& intervals) {
	printf("\n Calling ExpireOldIntervals()!\n");
	printf("Interval i: [%d %d]\n",i.first, i.second);
	for(set<ii, sortByEnd>::iterator j = active.begin(); 
								 j != active.end(); ) {
		// if the intervals start becoming relevant, stop!
		if (j -> second >= i.first) {
			printf("Stopping expiration procedures at interval [%d %d]\n", j->first, j->second);
			return;
		}
		printf("Will expire interval [%d %d] with register %d\n", j->first, j->second, regist[ intervals[*j] ]);
		// interval j is not irrelevant, so it frees up a register and 
		// gets removed from active
		int freed_register = regist [ intervals[*j] ];
		if (freed_register == 2) {
			regB = true;
		}
		else if (freed_register == 3) {
			regC = true;
		}
		else if (freed_register == 4) {
			regD = true;
		}
		j = active.erase(j);
	}
	printf("\n --- Done expiring intervals --- \n");
}

void SpillAtInterval(ii i, set<ii, sortByEnd>& active, map<LLVMValueRef, int>& regist, map<ii, LLVMValueRef, sortByStart>& intervals) {
	// what we spill is what finishes last: the last interval of active or i
	ii spill = *active.rbegin();
	if (spill.second > i.second) {
		// spill the last interval of active
		regist [ intervals[i] ] = regist [ intervals [spill] ];
		regist [ intervals[spill] ] = -1; // spill register
		active.erase (spill);
		active.insert (i);
	}
	else {
		// spill i
		regist [ intervals[i] ] = -1; // spill register
	}
}

/* ************************* */

/* Main register allocation algorithm */
map<LLVMValueRef, int> regAlloc (map<LLVMValueRef,ii >& live_range) {
	// make a list of the live intervals and their corresponding instructions,
	// taking care to sort the intervals by increasing start point
	map<ii, LLVMValueRef, sortByStart > intervals;
	for (map<LLVMValueRef, ii >::iterator it = live_range.begin();
													  it != live_range.end();
													  ++it) {
		LLVMValueRef inst = it->first;
		// we don't want to allocate space for store, branch, return and print!
		if (LLVMIsAStoreInst (inst) || LLVMIsAReturnInst(inst) || LLVMIsABranchInst(inst) || 
				(LLVMIsACallInst(inst) && LLVMGetNumArgOperands(inst) == 1))
				continue;
		intervals [it->second] = it->first;	
	}
	// we only deal with ''intervals'' from now on, not with ''live_range''.

	// set of live intervals that overlap current point and which 
	// have been placed in registers
	// |active| <= #registers at all times
	//
	// active is also sorted by increasing right endpoint
	set<ii, sortByEnd> active;
	
	// availability of registers
	regB=true; regC=true; regD=true;

	// register allocation results
	map<LLVMValueRef, int> regist;

	// go through all intervals in order of increasing start endpoint
	for (map<ii, LLVMValueRef>::iterator it = intervals.begin();
													 it != intervals.end();
													 ++it) {
		ii i = it->first;
		LLVMValueRef instr = it->second;
		printf("\n\t\tInterval: [%d %d]\n", i.first, i.second);
		// first get rid of all intervals which don't overlap with current point
		ExpireOldIntervals (i, active, regist, intervals);
		// if we have as many intervals as registers in active, we spill one interval
		// before we can add to active again
		if (active.size() == 3) {
			SpillAtInterval(i, active, regist, intervals);
		}
		else {
			// otherwise, allocate a register for the particular interval
			if (regB) {
				regist [instr] = 2;
				regB = false;
			} else if (regC) {
				regist [instr] = 3;
				regC = false;
			} else if (regD) {
				regist [instr] = 4;
				regD = false;
			}
			// expand the active set, while keeping it sorted
			printf("\nInserting new interval into active set\n");
			active.insert(i);
		}
		printf("\n\t\tContents of active:\n");
		for(set<ii, sortByEnd>::iterator j = active.begin();
													j != active.end();
													++j) {
			printf("[%d %d], ",j->first, j->second);
		}
		printf("\n\n");
	}
	return regist;
}
