//Constant propagation across basic blocks 

int f(){
	int i = 0;
	int j = 100;
	
	while(i < 1000) {
		i = i + j; 
	}

	return i;
}
