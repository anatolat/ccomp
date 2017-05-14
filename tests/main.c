#include <stdio.h>

int gg;


void testi() {
	++gg;
	++gg;
	--gg;

	printf("%d\n", gg++);
	printf("%d\n", gg--);
}
