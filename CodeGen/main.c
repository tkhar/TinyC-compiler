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
	int a = f(3);
	printf("%d\n",a);
}
