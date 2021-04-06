LLVMCODE = codegen
REGALLOC = reg_alloc
ASSGEN = assembly_gen
TEST = ''
RESULT = result

$(LLVMCODE): clean $(LLVMCODE).h $(REGALLOC).h $(ASSGEN).h
	g++ `llvm-config --cxxflags --ldflags --libs core` $(LLVMCODE).c $(REGALLOC).c $(ASSGEN).c -o $@

test:
	clang -S -emit-llvm $(TEST).c -o $(TEST).ll

asm: $(TEST).c
	gcc -S -m32 -fno-dwarf2-cfi-asm -fno-asynchronous-unwind-tables $(TEST).c -o $(TEST).s

myasm: $(LLVMCODE) test
	./$(LLVMCODE) $(TEST).ll
	gcc -g -m32 main.c outputAssembly.s

clean: 
	rm -rf $(TEST).ll
	rm -rf $(LLVMCODE)
	rm -rf *.o
	rm -rf *.s
	rm -rf a.out
