#include <stdio.h>

// Recursive function
int fact(int n) {
	if (n < 2) return 1;

	return n *  fact(n - 1);
}

/**
 */
void helloWorld() { printf("hello\tWorld\n"); }

void test0() {
	helloWorld();
}

// 
void test3() {
	printf("%d %d %d\n", 1, 2, 3);
}

char* testptr(char* ptr) {
	printf(ptr);
	return ptr;
}

int fwddecl(char* ptr);