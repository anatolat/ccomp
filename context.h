
// 
extern int cpool_size;
extern char cpool[65536];

extern int nint_consts;
extern char int_consts[256][64];
extern int int_consts_vals[256];

int add_str(const char* s);

int add_int_const(const char* name, int value);
int get_int_const(const char* name);

//
enum {
	ATTR_EXTERN = 1
};

extern char funcname[256];
extern int stack_size;

extern int nglobals;
extern char globals[256][64];
extern int global_vars[256][64];

int add_global(const char* s);
int get_global(const char* s);

