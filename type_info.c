#include  <stdio.h>
#include <memory.h>
#include "type_info.h"
#include "types.h"

int type_info_size;
int type_info[62];

void dump_type(int* type_info, int size) {
	int array_size = get_type_array_size(type_info, size);
	int basic_type = get_basic_type(type_info, size);
	printf("TYPE basic_type: %s, size %d\n", types[basic_type], array_size);

	for (int i = size - 1; i >= 0; --i) {
		switch (type_info[i]) {
		case DECL_ARRAY:
			printf("[%d] of ", type_info[--i]);
			break;

		case DECL_PTR:
			printf("ptr to ");
			break;

		case DECL_FUN:
			printf("function returning ");
			break;

		case DECL_BASIC:
			printf("%s\n", types[type_info[--i]]);
			break;
		}
	}
}

int get_type_byte_size(int* type_info, int size) {
	int result = 1;
	for (int i = size - 1; i >= 0; --i) {
		switch (type_info[i]) {
		case DECL_ARRAY:
			result *= type_info[--i];
			break;

		case DECL_FUN:
		case DECL_PTR:
			result *= 4;
			return result;

		case DECL_BASIC:
			result *= types_sizes[type_info[--i]];
			return result;
		}
	}

	return result;
}

int get_type_array_size(int* type_info, int size) {
	int result = 1;
	for (int i = size - 1; i >= 0; --i) {
		switch (type_info[i]) {
		case DECL_ARRAY:
			result *= type_info[--i];
			break;

		case DECL_FUN:
		case DECL_PTR:
		case DECL_BASIC:
			return result;
		}
	}

	return result;
}

int get_basic_type(int* type_info, int size) {
	return type_info[0];
}

void get_item_type_info(int* dest_type_info, int* dest_size, int* type_info, int type_info_size) {
	switch (type_info[type_info_size - 1]) {
	case DECL_ARRAY:
		*dest_size = type_info_size - 2;
		break;

	case DECL_FUN:
	case DECL_PTR:
		*dest_size = type_info_size - 1;
		break;

	case DECL_BASIC:
		*dest_size = type_info_size;
		break;
	}
	memcpy(dest_type_info, type_info, *dest_size * sizeof(type_info[0]));
}

void set_int_type() {
	type_info_size = 0;
	type_info[type_info_size++] = TYPE_INT;
	type_info[type_info_size++] = DECL_BASIC;
}