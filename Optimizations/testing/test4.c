//Constant propagation across basic blocks. The first round of constant
//propagation will create an oppotunity for constant folding and 
//another round of constant propagation.

int f(){
	int i = 0;
	int j = 100;
	
	while(i < 1000) {
		int k = j + j;
		i = i + k; 
	}

	return i;
}
