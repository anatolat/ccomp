#include <stdio.h>

int gg;


void testi(int a, int b) {
	printf("a == b: %d\na != b: %d\na > b: %d\na < b: %d\na >= b: %d\na <= b: %d\n",
		 a == b, a != b,
		a > b, a < b, a >= b, a <= b);
}
