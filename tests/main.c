#include <stdio.h>

int testi(int* ar, int i) {
	ar[i] = 1;
	return ar[i];
}

int main() {
	printf("Hello, World!\n");
}