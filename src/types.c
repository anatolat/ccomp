#include "types.h"

int ntypes;
const char* types[256];
int types_sizes[256];


int add_type(const char* s, int size) {
	int id = ntypes++;
	types[id] = s;
	types_sizes[id] = size;

	return id;
}
