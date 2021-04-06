extern void print (int);
extern int read(void);

int f(int a) {
	a = 0;
	while (a < 10) {
		int i; 
		i = 10 + 10;
		int b;
		b = i + 10;
		int c;
		int d;
		d = read();
		print (d);
		c = i + i;
		a = a + 1;
		print (a);
	}
	return 0;
}
