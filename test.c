extern int read(void);
extern void print(int);

int f (int a) {
	int i;
	i = 0;
	while (i < 10) {
		if (i < 5) {
			print (i);
		}
		i = i +1;
	}
	return (1);
}
