int func1(int a, int b, int n){
	int i, j;

	i = a + b;
	if (b < a)
		i = a;
	else
		i = b;

	while(i < 20){
		i = i + 1;
	}
	return i;
}

