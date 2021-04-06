#include <stdio.h>

void print (int a) {
	printf("%d\n",a);
}

int read () {
	int a;
	scanf("%d",&a);
	return a;
}

int f (int);

int main() {
	printf("%d\n",f(10));
}
