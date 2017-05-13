#include <stdio.h>

// Recursive function
int fact(int n) {
	if (n < 2) return 1;

	return n *  fact(n - 1);
}

void test() {
	printf("%d  %d %d", ' ', 'a', '\n');
}