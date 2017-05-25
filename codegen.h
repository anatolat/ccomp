// Call format:
//    OP_CALL header_pointer <...>
//    header: param_count param_offset0 param_offset1 ...
enum {
	OP_FUNC_PROLOGUE
	, OP_FUNC_EPILOGUE
	, OP_LOAD
	, OP_ADDR
	, OP_PUSH
	, OP_DEREF
	, OP_DEREF_ADDR
	, OP_SAVE
	, OP_CALL
	, OP_RETURN
	, OP_TEST
	, OP_JMP
	, OP_JMPZ
	, OP_JMPNZ
	, OP_LABEL
	, OP_ADD
	, OP_SUB
	, OP_MUL
	, OP_DIV
	, OP_MOD
	, OP_LESS
	, OP_GREATER
	, OP_LESS_EQ
	, OP_GREATER_EQ
	, OP_EQ
	, OP_NOT_EQ
	, OP_INC
	, OP_DEC
	, OP_INC_POST
	, OP_DEC_POST
	, OP_NOT
	, OP_NEG
	, OP_DUP_VALUE
	, OP_FOR
	, OP_CASE
};

enum {
	VAL_STR,
	VAL_INT,
	VAL_LOCAL,
	VAL_LOCAL_ADDR,
	VAL_GLOB,
	VAL_GLOB_ADDR
};

extern int nopcodes;
extern int opcodes[65536];

int emit(int op);
void emit_push(int val_type, int val, int size);


void gen_code(FILE* ftarget, const char* funcname, int from, int end);