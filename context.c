#include <string.h>
#include "context.h"
#include "type_info.h"

int cpool_size;
char cpool[65536];

int nint_consts;
char int_consts[256][64];
int int_consts_vals[256];

char funcname[256];
int stack_size;

int nglobals;
char globals[256][64];
int global_vars[256][64];

int add_str(const char* s) {
	int size = strlen(s) + 1;

	memcpy(cpool + cpool_size, s, size);
	cpool_size += size;
	return cpool_size - size;
}

int add_int_const(const char* name, int val) {
	int id = nint_consts++;
	strcpy(int_consts[id], name);
	int_consts_vals[id] = val;

	return id;
}

int get_int_const(const char* s) {
	for (int i = nint_consts - 1; i >= 0; --i) {
		if (!strcmp(int_consts[i], s)) return i;
	}
	return -1;
}


int add_global(const char* s) {
	int index = nglobals++;
	strcpy(globals[index], s);

	global_vars[index][1] = type_info_size;
	memcpy(&global_vars[index][2], type_info, type_info_size * sizeof(type_info[0]));

	return index;
}

int get_global(const char* s) {
	for (int i = 0; i < nglobals; ++i) {
		if (!strcmp(globals[i], s)) return i;
	}
	return -1;
}
