#include <stdio.h>
#include <string.h>


char strs[10][256];


int main() {
	for (int i = 0; i < 10; ++i) {
		strcpy(strs[i], "hello: ");
		for  (int j = 0; j < 10; ++j) {
			strs[i][j + 7] = '0' + j;
		}
	}

	for (int i = 0; i < 10; ++i) {
		printf("%s\n", strs[0]);
	}
}