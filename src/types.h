
extern int ntypes;
extern const char* types[256];
extern int types_sizes[256];

enum {
	TYPE_VOID,
	TYPE_CHAR,
	TYPE_INT
};

int add_type(const char* s, int size);