#include <string.h>
#include "context.h"
#include "type_info.h"

char funcname[256];
int stack_size;

int nglobals;
char globals[256][64];
int global_vars[256][64];



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
