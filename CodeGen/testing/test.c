extern int read();
extern void print(int);

int f(int a){
	int i;
	int c;
	i = 1;
	a = 0;
	c = read();
	print (c);
	print (a);
	print (i);
	while (i < 10) {
		print (i);
		a = a + 2;
		i = i + 1;
	}
	print (a);
	return i;
}
