LLVMCODE = optimizations
TESTFILE=test1
TEST = testing/$(TESTFILE)

$(LLVMCODE): clean $(LLVMCODE).c
	c++ `llvm-config --cxxflags --ldflags --libs core` $(LLVMCODE).c -o $@

test: $(TEST).c
	clang -S -emit-llvm $(TEST).c -o $(TEST).ll

clean: 
	rm -rf $(TEST).ll
	rm -rf $(LLVMCODE)
	rm -rf *.o
	rm -rf core
	rm -rf testOpt.ll
	rm -rf a.out
