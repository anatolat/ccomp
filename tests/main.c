#include <stdio.h>

void test(int a, int b) {
	for (int i = 0; i < b; i = i + 1) {
		printf("%d\n", i);
	}
}
