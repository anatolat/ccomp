
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

