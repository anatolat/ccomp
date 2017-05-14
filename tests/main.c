#include <stdio.h>

int gg;


int fa(int a) {
	printf("fa\n");
	return a;
}

int fb(int b) {
	printf("fb\n");
	return b;
}

void testi(int a, int b) {
	printf("result: %d\n", fa(a) && fb(b));
}
