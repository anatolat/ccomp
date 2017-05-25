#include <stdio.h>
#include <string.h>

FILE* fsource;
FILE* ftarget;

void preprocess(const char* source, const char* target) {
	fsource = fopen(source, "r");
	ftarget = fopen(target, "w");



	fclose(ftarget);
	fclose(fsource);
}