#include <stdio.h>

void looptest() {
	int i;
	i = 0;
	while (i < 10) {
		i = i + 1;	
		printf("hello, world %d\n", i);
	}
}