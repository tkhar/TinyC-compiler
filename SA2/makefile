LLVMCODE = llvm_parser
TEST = test

$(LLVMCODE): $(LLVMCODE).c
	cc `llvm-config --cflags` -c $(LLVMCODE).c
	c++ `llvm-config --cxxflags --ldflags --libs core` $(LLVMCODE).o -o $@

test: $(TEST).c
	clang -S -emit-llvm $(TEST).c -o $(TEST).ll

clean: 
	rm -rf $(TEST).ll
	rm -rf $(LLVMCODE)
	rm -rf *.o
