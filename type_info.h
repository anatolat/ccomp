
enum {
	DECL_ARRAY = 1,
	DECL_PTR,
	DECL_FUN,
	DECL_BASIC
};


extern int type_info_size;
extern int type_info[62];

void dump_type(int* type_info, int size);
int get_basic_type(int* type_info, int size);
int get_type_array_size(int* type_info, int size);
int get_type_byte_size(int* type_info, int size);
void get_item_type_info(int* dest_type_info, int* dest_size, int* type_info, int type_info_size);


void set_int_type();

