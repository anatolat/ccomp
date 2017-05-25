#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

enum {
	T_EOF
	, T_ID
	, T_TYPEID
	, T_CHAR_LIT
	, T_INT_LIT
	, T_STR_LIT

	, T_SEMI
	, T_COMMA
	, T_PERIOD
	, T_COLON
	, T_QUESTION

	, T_ASSIGNMENT
	, T_ASSIGNMENT_ADD
	, T_ASSIGNMENT_SUB
	, T_ASSIGNMENT_MUL
	, T_ASSIGNMENT_DIV
	, T_ASSIGNMENT_MOD
	, T_ADD
	, T_SUB
	, T_MUL
	, T_DIV
	, T_MOD
	, T_LESS
	, T_GREATER
	, T_LESS_EQ
	, T_GREATER_EQ
	, T_EQ
	, T_NOT_EQ
	, T_OR
	, T_AND
	, T_BIT_OR
	, T_BIT_AND
	, T_BIT_XOR
	, T_SHL
	, T_SHR

	, T_NOT
	, T_INC
	, T_DEC

	, T_LPAREN // (
	, T_RPAREN // )
	, T_LBRACKET // [
	, T_RBRACKET // ]
	, T_LCURLY // {
	, T_RCURLY // {

	// keywords
	, T_RETURN
	, T_IF
	, T_ELSE
	, T_WHILE
	, T_DO
	, T_FOR
	, T_BREAK
	, T_CONTINUE
	, T_SWITCH
	, T_CASE
	, T_DEFAULT
	, T_ENUM
	, T_CONST
	, T_SIZEOF
};

FILE* fsource;
FILE* ftarget;
int token;
char token_id[256];
int token_num;
int lineno;

int ntypes;
const char* types[256];
int types_sizes[256];

enum {
	TYPE_VOID,
	TYPE_CHAR,
	TYPE_INT
};

int add_type(const char* s, int size);


// cpool
int cpool_size;
char cpool[65536];

int nint_consts;
char int_consts[256][64];
int int_consts_vals[256];

int add_str(const char* s);

int add_int_const(const char* name, int value);
int get_int_const(const char* name);

// end cpool

// codegen
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

enum {
	DECL_ARRAY = 1,
	DECL_PTR,
	DECL_FUN,
	DECL_BASIC
};

enum {
	SCOPE_BLOCK,
	SCOPE_LOOP,
	SCOPE_SWITCH
};

enum {
	ATTR_EXTERN = 1
};

int nopcodes;
int opcodes[65536];

int emit(int op);
void emit_push(int val_type, int val, int size);

char funcname[256];
int stack_size;

int nscopes;
int scopes[64][8];

int start_block_scope();
int start_switch_scope(int end_label, int case_label, int var);
int start_loop_scope(int break_label, int continue_label);
void end_scope();
int find_scope(int type);
int find_break_label();
int find_continue_label();

int nparams;
char params[256][64];

int nlocals;
char locals[256][64];
int local_vars[256][64];

int nlabels;

int nglobals;
char globals[256][64];
int global_vars[256][64];

int last_value_ref;

int type_info_size;
int type_info[62];
	
void dump_type(int* type_info, int size);
int get_basic_type(int* type_info, int size);
int get_type_array_size(int* type_info, int size);
int get_type_byte_size(int* type_info, int size);
void get_item_type_info(int* dest_type_info, int* dest_size, int* type_info, int type_info_size);
void set_int_type();

int add_param(const char* s);
int add_local(const char* s);
int add_global(const char* s);

int get_local(const char* s);
int get_param(const char* s);
int get_extern(const char* s);
int get_global(const char* s);

void start_func_decl(const char* s);
void start_func_body();
void end_func();
// end

// asmgen
void gen_code(int from, int end);
// end


// scanner
const char* tok2str(int tok) {
	switch (tok) {
	case T_EOF: return "EOF";
	case T_ID: return "ID";
	case T_TYPEID: return "TYPEID";
	case T_CHAR_LIT: return "CHAR_LIT";
	case T_INT_LIT: return "INT_LIT";
	case T_STR_LIT: return "STR_LIT";

	case T_SEMI: return "SEMI";
	case T_COMMA: return "COMMA";
	case T_PERIOD: return "PERIOD";
	case T_COLON: return "COLON";
	case T_QUESTION: return "QUESTION";

	case T_ASSIGNMENT: return "ASSIGNMENT";
	case T_ASSIGNMENT_ADD: return "ASSIGNMENT_ADD";
	case T_ASSIGNMENT_SUB: return "ASSIGNMENT_SUB";
	case T_ASSIGNMENT_MUL: return "ASSIGNMENT_MUL";
	case T_ASSIGNMENT_DIV: return "ASSIGNMENT_DIV";
	case T_ASSIGNMENT_MOD: return "ASSIGNMENT_MOD";
	case T_EQ: return "EQ";
	case T_NOT_EQ: return "NOT-EQ";
	case T_ADD: return "ADD";
	case T_SUB: return "SUB";
	case T_MUL: return "MUL";
	case T_DIV: return "DIV";
	case T_MOD: return "MOD";
	case T_LESS: return "LESS";
	case T_GREATER: return "GREATER";
	case T_LESS_EQ: return "LESS-EQ";
	case T_GREATER_EQ: return "GREATER-EQ";
	case T_OR: return "OR";
	case T_AND: return "AND";
	case T_BIT_OR: return "BIT_OR";
	case T_BIT_AND: return "BIT_AND";
	case T_BIT_XOR: return "BIT_XOR";
	case T_SHL: return "SHL";
	case T_SHR: return "SHR";
	case T_NOT: return "NOT";
	case T_INC: return "INC";
	case T_DEC: return "DEC";

	case T_LPAREN: return "LPAREN";
	case T_RPAREN: return "RPAREN";
	case T_LBRACKET: return "LBRACKET";
	case T_RBRACKET: return "RBRACKET";
	case T_LCURLY: return "LCURLY";
	case T_RCURLY: return "RCURLY";

	case T_RETURN: return "RETURN";
	case T_IF: return "IF";
	case T_ELSE: return "ELSE";
	case T_WHILE: return "WHILE";
	case T_DO: return "DO";
	case T_FOR: return "FOR";
	case T_BREAK: return "BREAK";
	case T_CONTINUE: return "CONTINUE";
	case T_SWITCH: return "SWITCH";
	case T_CASE: return "CASE";
	case T_DEFAULT: return "DEFAULT";
	case T_ENUM: return "ENUM";
	case T_CONST: return "CONST";
	case T_SIZEOF: return "SIZEOF";
	}
	return "XXX";
}


int next_token_helper() {
	int ch;

	do {
		ch = fgetc(fsource);
		if (ch == '\n') ++lineno;

		// ignore preprocessor directives
		if (ch == '#') {
			while (ch != '\n' && ch != EOF) {
				ch = fgetc(fsource);
			}

			if (ch == '\n') ++lineno;
		}
		// comments
		else if (ch == '/') {
			ch = fgetc(fsource);
			if (ch == '/') {
				while (ch != '\n' && ch != EOF) {
					ch = fgetc(fsource);
				}

				if (ch == '\n') ++lineno;
			}
			else if (ch == '*') {
				while (ch != EOF) {
					ch = fgetc(fsource);
					if (ch == '\n') ++lineno;

					if (ch == '*') {
						ch = fgetc(fsource);
						if (ch == '/') {
							ch = ' ';
							break;
						}

						ungetc(ch, fsource);
					}
				}
			}
			else {
				ungetc(ch, fsource);
				ch = '/';
			}
		}

	} while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

	token_id[0] = 0;

	if (ch == EOF) return T_EOF;
	if (ch == ';') return T_SEMI;
	if (ch == ',') return T_COMMA;
	if (ch == '.') return T_PERIOD;
	if (ch == ':') return T_COLON;
	if (ch == '?') return T_QUESTION;

	if (ch == '=') {
		ch = fgetc(fsource);
		if (ch == '=') return T_EQ;

		ungetc(ch, fsource);
		return T_ASSIGNMENT;
	}
	if (ch == '+') {
		ch = fgetc(fsource);
		if (ch == '+') return T_INC;
		if (ch == '=') return T_ASSIGNMENT_ADD;

		ungetc(ch, fsource);
		return T_ADD;
	}
	if (ch == '-') {
		ch = fgetc(fsource);
		if (ch == '-') return T_DEC;
		if (ch == '=') return T_ASSIGNMENT_SUB;

		ungetc(ch, fsource);
		return T_SUB;
	}
	if (ch == '*') {
		ch = fgetc(fsource);
		if (ch == '=') return T_ASSIGNMENT_MUL;

		ungetc(ch, fsource);
		return T_MUL;
	}
	if (ch == '%') {
		ch = fgetc(fsource);
		if (ch == '=') return T_ASSIGNMENT_MOD;

		ungetc(ch, fsource);
		return T_MOD;
	}
	if (ch == '/') {
		ch = fgetc(fsource);
		if (ch == '=') return T_ASSIGNMENT_DIV;

		ungetc(ch, fsource);
		return T_DIV;
	}
	if (ch == '<') {
		ch = fgetc(fsource);
		if (ch == '=') return T_LESS_EQ;

		ungetc(ch, fsource);
		return T_LESS;
	}
	if (ch == '>') {
		ch = fgetc(fsource);
		if (ch == '=') return T_GREATER_EQ;

		ungetc(ch, fsource);
		return T_GREATER;
	}

	if (ch == '|') {
		ch = fgetc(fsource);
		if (ch == '|') return T_OR;

		ungetc(ch, fsource);
		return T_BIT_OR;
	}

	if (ch == '&') {
		ch = fgetc(fsource);
		if (ch == '&') return T_AND;

		ungetc(ch, fsource);
		return T_BIT_AND;
	}

	if (ch == '!') {
		ch = fgetc(fsource);
		if (ch == '=') return T_NOT_EQ;

		ungetc(ch, fsource);
		return T_NOT;
	}

	if (ch == '(') return T_LPAREN;
	if (ch == ')') return T_RPAREN;
	if (ch == '[') return T_LBRACKET;
	if (ch == ']') return T_RBRACKET;
	if (ch == '{') return T_LCURLY;
	if (ch == '}') return T_RCURLY;

	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {

		int i = 0;
		do {
			token_id[i++] = ch;
			ch = fgetc(fsource);
		} while (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == '_');

		token_id[i] = 0;

		int t = T_ID;
		if (!strcmp(token_id, "return")) {
			t = T_RETURN;
		}
		else if (!strcmp(token_id, "if")) {
			t = T_IF;
		}
		else if (!strcmp(token_id, "while")) {
			t = T_WHILE;
		}
		else if (!strcmp(token_id, "do")) {
			t = T_DO;
		}
		else if (!strcmp(token_id, "for")) {
			t = T_FOR;
		}
		else if (!strcmp(token_id, "else")) {
			t = T_ELSE;
		}
		else if (!strcmp(token_id, "break")) {
			t = T_BREAK;
		}
		else if (!strcmp(token_id, "continue")) {
			t = T_CONTINUE;
		}
		else if (!strcmp(token_id, "switch")) {
			t = T_SWITCH;
		}
		else if (!strcmp(token_id, "case")) {
			t = T_CASE;
		}
		else if (!strcmp(token_id, "default")) {
			t = T_DEFAULT;
		}
		else if (!strcmp(token_id, "enum")) {
			t = T_ENUM;
		}
		else if (!strcmp(token_id, "const")) {
			t = T_CONST;
		}
		else if (!strcmp(token_id, "sizeof")) {
			t = T_SIZEOF;
		}
		else {
			for (int i = 0; i < ntypes; ++i) {
				if (!strcmp(types[i], token_id)) {
					t = T_TYPEID;
					token_num = i;
					break;
				}
			}
		}

		ungetc(ch, fsource);
		return t;
	}

	if (ch >= '0' && ch <= '9') {
		int n = 0;
		do {
			n = n * 10 + (ch - '0');
			ch = fgetc(fsource);
		} while (ch >= '0' && ch <= '9');

		token_num = n;
		ungetc(ch, fsource);
		return T_INT_LIT;
	}

	if (ch == '"') {
		int i = 0;
		ch = fgetc(fsource);
		while (ch != '"' && ch != EOF) {
			if (ch == '\\') {
				ch = fgetc(fsource);

				if (ch == EOF) continue;

				if (ch == 'n') ch = '\n';
				else if (ch == 'r') ch = '\r';
				else if (ch == 't') ch = '\t';
			}

			token_id[i++] = ch;
			ch = fgetc(fsource);
		}

		token_id[i] = 0;
		return T_STR_LIT;
	}

	if (ch == '\'') {
		// TODO
		ch = fgetc(fsource);
		if (ch == '\\') {
			ch = fgetc(fsource);

			if (ch == 'n') ch = '\n';
			else if (ch == 'r') ch = '\r';
			else if (ch == 't') ch = '\t';
		}
		token_num = ch;

		fgetc(fsource);
		return T_CHAR_LIT;
	}


	//fprintf(stderr, "Syntax error (%d): invalid token %c\n", lineno, ch);
	printf("Syntax error (%d): invalid token %c\n", lineno, ch);
	return T_EOF;
}

void next_token() {
	token = next_token_helper();
}

// end scanner

// parser
enum {
	DECL_CTX_DEFAULT,
	DECL_CTX_PARAM,
	DECL_CTX_FOR_INIT
};

void parse_assignment_expr();
void parse_stmts(int terminator);
void parse_declaration(int ctx);


int check_token(int expected) {
	if (token != expected) {
		//fprintf(stderr, "Syntax error (%d): unexpected token %s, expected %s\n", lineno, tok2str(token), tok2str(expected));
		printf("Syntax error (%d): unexpected token %s, expected %s\n", lineno, tok2str(token), tok2str(expected));
		return 0;
	}
	return 1;
}

void parse_token(int expected) {
	check_token(expected);
	next_token();
}

int try_parse_token(int expected) {
	if (token == expected) {
		next_token();
		return 1;
	}
	return 0;
}

void parse_primary_expr() {
	last_value_ref = nopcodes;
	if (token == T_INT_LIT) {		
		emit_push(VAL_INT, token_num, 0);

		type_info_size = 0;
		type_info[type_info_size++] = TYPE_INT;
		type_info[type_info_size++] = DECL_BASIC;

		next_token();
		return;
	}
	if (token == T_STR_LIT) {
		int cid = add_str(token_id);
		emit_push(VAL_STR, cid, 0);

		type_info_size = 0;
		type_info[type_info_size++] = TYPE_CHAR;
		type_info[type_info_size++] = DECL_BASIC;
		type_info[type_info_size++] = DECL_PTR;

		next_token();
		return;
	}
	if (token == T_CHAR_LIT) {
		emit_push(VAL_INT, token_num, 0);

		type_info_size = 0;
		type_info[type_info_size++] = TYPE_INT;
		type_info[type_info_size++] = DECL_BASIC;

		next_token();
		return;
	}
	if (token == T_LPAREN) {
		next_token();

		parse_assignment_expr();

		parse_token(T_RPAREN);
		return;
	}
	if (check_token(T_ID)) {
		int id = get_local(token_id);
		if (id != -1) {
			int offs = local_vars[id][0];

			type_info_size = local_vars[id][1];
			memcpy(type_info, &local_vars[id][2], type_info_size * sizeof(type_info[0]));

			int is_addr = type_info[type_info_size - 1] == DECL_ARRAY;
			int size = get_type_byte_size(type_info, type_info_size);

			emit_push(is_addr ? VAL_LOCAL_ADDR : VAL_LOCAL, offs, size);
		}
		else if ((id = get_param(token_id)) != -1) {
			emit_push(VAL_LOCAL, 4 * (id + 2), 0);

			set_int_type();
		}
		else if ((id = get_global(token_id)) != -1) {
			type_info_size = global_vars[id][1];
			memcpy(type_info, &global_vars[id][2], type_info_size * sizeof(type_info[0]));

			int is_addr = 
				type_info[type_info_size - 1] == DECL_ARRAY ||
				type_info[type_info_size - 1] == DECL_FUN;

			emit_push(is_addr ? VAL_GLOB_ADDR : VAL_GLOB, id, 0);
		}
		else if ((id = get_int_const(token_id)) != -1) {
			int val = int_consts_vals[id];
			set_int_type();

			emit_push(VAL_INT, val, 4);
		}
		else {
			//fprintf(stderr, "Error (%d): unknown identifier %s\n", lineno, token_id);
			printf("Error (%d): unknown identifier %s\n", lineno, token_id);
		}
		next_token();
		return;
	}
	
	next_token();
}

void convert_to_addr(int ref) {
	if (opcodes[ref] == OP_PUSH && opcodes[ref + 1] == VAL_LOCAL) {
		opcodes[ref + 1] = VAL_LOCAL_ADDR;
	}
	else if (opcodes[ref] == OP_PUSH && opcodes[ref + 1] == VAL_GLOB) {
		opcodes[ref + 1] = VAL_GLOB_ADDR;
	}
	else if (opcodes[ref] == OP_DEREF) {
		opcodes[ref] = OP_DEREF_ADDR;
	}
}

void parse_postfix_expr() {
	parse_primary_expr();

	while (token != T_EOF) {
		if (token == T_LPAREN) {
			next_token();

			last_value_ref = nopcodes;
			emit(OP_CALL);
			int sizes[256]; // size of the code generated for each parameter
			int call_header = emit(0);

			int param_count = 0;
			while (token != T_RPAREN && token != T_EOF) {
				sizes[param_count++] = nopcodes;

				parse_assignment_expr();

				if (token == T_COMMA) {
					next_token();
					continue;
				}
			}

			parse_token(T_RPAREN);

			opcodes[call_header] = emit(param_count);
			for (int i = 0; i < param_count; ++i) {
				emit(sizes[i]);
			}

			set_int_type();
		}
		else if (token == T_LBRACKET) {
			next_token();

			int item_type_info[64];
			int item_type_info_size;

			//dump_type(type_info, type_info_size);
			get_item_type_info(item_type_info, &item_type_info_size, type_info, type_info_size);

			parse_assignment_expr();

			int is_addr = item_type_info[item_type_info_size - 1] == DECL_ARRAY;
			last_value_ref = nopcodes;

			emit(is_addr ? OP_DEREF_ADDR : OP_DEREF);
			emit(get_type_byte_size(item_type_info, item_type_info_size));

			parse_token(T_RBRACKET);

			type_info_size = item_type_info_size;
			memcpy(type_info, item_type_info, item_type_info_size * sizeof(type_info[0]));
		}
		else if (token == T_INC || token == T_DEC) {
			int inc = token == T_INC;
			next_token();

			convert_to_addr(last_value_ref);

			last_value_ref = nopcodes;
			emit(inc ? OP_INC_POST : OP_DEC_POST);

			set_int_type();
			break;
		}
		else break;
	}
}

void parse_unary_expr() {
	if (token == T_INC || token == T_DEC) {
		int inc = token == T_INC;
		next_token();
		
		parse_unary_expr();

		convert_to_addr(last_value_ref);
		last_value_ref = nopcodes;

		emit(inc ? OP_INC : OP_DEC);

		set_int_type();
		return;
	}
	if (token == T_NOT) {
		next_token();

		parse_unary_expr();

		last_value_ref = nopcodes;
		emit(OP_NOT);

		set_int_type();
		return;
	}
	if (token == T_ADD) {
		next_token();
		
		parse_unary_expr();

		set_int_type();
		return;
	}
	if (token == T_SUB) {
		next_token();

		parse_unary_expr();

		last_value_ref = nopcodes;
		emit(OP_NEG);

		set_int_type();
		return;
	}
	if (token == T_BIT_AND) {
		next_token();

		parse_unary_expr();

		convert_to_addr(last_value_ref);

		set_int_type(); // FIXME
		return;
	}
	if (token == T_MUL) {
		next_token();

		parse_unary_expr();

		int item_type_info[64];
		int item_type_info_size;

		get_item_type_info(item_type_info, &item_type_info_size, type_info, type_info_size);

		// TODO optimize
		emit_push(VAL_INT, 0, 0);

		last_value_ref = nopcodes;
		emit(OP_DEREF);
		emit(get_type_byte_size(item_type_info, item_type_info_size));

		type_info_size = item_type_info_size;
		memcpy(type_info, item_type_info, item_type_info_size * sizeof(type_info[0]));

		//dump_type(type_info, type_info_size);
		return;
	}
	if (token == T_SIZEOF) {
		next_token();
		
		int code = nopcodes;
		parse_unary_expr();

		nopcodes = code;

		int result = get_type_byte_size(type_info, type_info_size);
		emit_push(VAL_INT, result, 0);
		return;
	}

	parse_postfix_expr();
}

int op_prec(int op) {
	switch (op) {
	case T_MUL: case T_DIV: case T_MOD:
		return 12;

	case T_ADD: case T_SUB: 
		return 11;

	case T_SHL: case T_SHR:
		return 10;

	case T_LESS: case T_GREATER:
	case T_LESS_EQ: case T_GREATER_EQ:
		return 9;	

	case T_EQ: case T_NOT_EQ:
		return 8;

	case T_BIT_AND:
		return 7;

	case T_BIT_XOR:
		return 6;

	case T_BIT_OR:
		return 5;

	case T_AND:
		return 4;

	case T_OR:
		return 3;

	case T_QUESTION:
		return 2;

	case T_COLON:
		return -1;

	case T_ASSIGNMENT:
	case T_ASSIGNMENT_ADD:
	case T_ASSIGNMENT_SUB:
	case T_ASSIGNMENT_MUL:
	case T_ASSIGNMENT_DIV:
	case T_ASSIGNMENT_MOD:
		return 1;

	default:
		return -1;
	}
}

void emit_jmpnz(int label) {
	emit(OP_JMPNZ);
	emit(label);
}

void emit_jmpz(int label) {
	emit(OP_JMPZ);
	emit(label);
}

void emit_save(int lvalue_size) {
	emit(OP_SAVE);
	emit(lvalue_size);
}

void emit_compound_save(int op, int lvalue_size) {
	emit(op);
	emit_save(lvalue_size);
}

void finish_or_expr(int or_label)  {
	emit_jmpnz(or_label);
	emit_push(VAL_INT, 0, 0);

	int end = nlabels++;
	emit(OP_JMP);
	emit(end);

	emit(OP_LABEL);
	emit(or_label);

	emit_push(VAL_INT, 1, 0);

	emit(OP_LABEL);
	emit(end);
}

void finish_and_expr(int and_label) {
	emit_jmpz(and_label);
	emit_push(VAL_INT, 1, 0);

	int end = nlabels++;
	emit(OP_JMP);
	emit(end);

	emit(OP_LABEL);
	emit(and_label);

	emit_push(VAL_INT, 0, 0);

	emit(OP_LABEL);
	emit(end);
}

void parse_expr(int min_prec) {
	parse_unary_expr();

	int or_label = -1;
	int and_label = -1;

	while (1) {
		int prec = op_prec(token);
		if (prec < min_prec) break;

		int op = token;
		next_token();

		if (op != T_OR && or_label != -1) {
			finish_or_expr(or_label);
			or_label = -1;
		}
		if (op != T_AND && and_label != -1) {
			finish_and_expr(and_label);
			and_label = -1;
		}

		if (op == T_OR) {
			if (or_label == -1) or_label = nlabels++;
			emit_jmpnz(or_label);
		}
		else if (op == T_AND) {
			if (and_label == -1) and_label = nlabels++;
			emit_jmpz(and_label);
		}

		int lvalue_ref = last_value_ref;
		int lvalue_size = get_type_byte_size(type_info, type_info_size);

		int q_zero_label = -1;
		int q_end_label = -1;

		if (op == T_ASSIGNMENT) {
			convert_to_addr(lvalue_ref);
		}
		else if (op == T_ASSIGNMENT_ADD || op == T_ASSIGNMENT_SUB ||
			op == T_ASSIGNMENT_MUL || op == T_ASSIGNMENT_DIV || op == T_ASSIGNMENT_MOD) {

			convert_to_addr(lvalue_ref);

			// get value and save to the stack
			emit(OP_DUP_VALUE);
			emit(lvalue_size);
		}
		
		if (op == T_QUESTION) {
			q_zero_label = nlabels++;
			q_end_label = nlabels++;
			emit_jmpz(q_zero_label);

			parse_expr(1);

			emit(OP_JMP);
			emit(q_end_label);

			emit(OP_LABEL);
			emit(q_zero_label);

			parse_token(T_COLON);
		}
		parse_expr(prec <= 2 ? prec : prec + 1);

		if (op == T_OR || op == T_AND) {
			// do nothing
		}
		else if (op == T_QUESTION) {
			emit(OP_LABEL);
			emit(q_end_label);
		}
		else if (op == T_ASSIGNMENT) emit_save(lvalue_size);
		else if (op == T_ASSIGNMENT_ADD) emit_compound_save(OP_ADD, lvalue_size);
		else if (op == T_ASSIGNMENT_SUB) emit_compound_save(OP_SUB, lvalue_size);
		else if (op == T_ASSIGNMENT_MUL) emit_compound_save(OP_MUL, lvalue_size);
		else if (op == T_ASSIGNMENT_DIV) emit_compound_save(OP_DIV, lvalue_size);
		else if (op == T_ASSIGNMENT_MOD) emit_compound_save(OP_MOD, lvalue_size);
		else if (op == T_ADD) emit(OP_ADD);
		else if (op == T_SUB) emit(OP_SUB);
		else if (op == T_MUL) emit(OP_MUL);
		else if (op == T_DIV) emit(OP_DIV);
		else if (op == T_MOD) emit(OP_MOD);
		else if (op == T_LESS) emit(OP_LESS);
		else if (op == T_GREATER) emit(OP_GREATER);
		else if (op == T_LESS_EQ) emit(OP_LESS_EQ);
		else if (op == T_GREATER_EQ) emit(OP_GREATER_EQ);
		else if (op == T_EQ) emit(OP_EQ);
		else if (op == T_NOT_EQ) emit(OP_NOT_EQ);
		else {
			//fprintf(stderr, "Internal error (%d): operator not supported %s\n", lineno, tok2str(op));
			printf("Internal error (%d): operator not supported %s\n", lineno, tok2str(op));
		}
	}

	if (or_label != -1) {
		finish_or_expr(or_label);
	}

	if (and_label != -1) {
		finish_and_expr(and_label);
	}
}

void parse_assignment_expr() {
	parse_expr(1);
}

void parse_func_params() {
	next_token();
	while (token != T_RPAREN && token != T_EOF) {

		parse_declaration(DECL_CTX_PARAM);

		if (token != T_COMMA) {
			break;
		}

		next_token();
	}

	check_token(T_RPAREN);
}

void parse_decl_spec() {
	// consts are not supported yet
	try_parse_token(T_CONST);

	check_token(T_TYPEID);
	
	type_info_size = 0;
	type_info[type_info_size++] = token_num;
	type_info[type_info_size++] = DECL_BASIC;

	next_token();
}

// declaration_specifiers
void parse_decl_specs() {
	parse_decl_spec();
}

/*
directDeclarator
:   Identifier
|   '(' declarator ')'
|   directDeclarator '[' typeQualifierList? assignmentExpression? ']'
|   directDeclarator '[' 'static' typeQualifierList? assignmentExpression ']'
|   directDeclarator '[' typeQualifierList 'static' assignmentExpression ']'
|   directDeclarator '[' typeQualifierList? '*' ']'
|   directDeclarator '(' parameterTypeList ')'
| directDeclarator '(' identifierList? ')'
 */
void parse_direct_declarator(char* id, int* func) {
	*func = 0;
	if (!check_token(T_ID)) return;

	strcpy(id, token_id);
	next_token();
	
	int tmp_type_info_size = 0;
	int tmp_type_info[63];

	while (1) {
		if (token == T_LBRACKET) {
			next_token();

			check_token(T_INT_LIT);
			tmp_type_info[tmp_type_info_size++] = DECL_ARRAY;
			tmp_type_info[tmp_type_info_size++] = token_num;

			next_token();

			parse_token(T_RBRACKET);
		}
		else if (token == T_LPAREN) {
			*func = 1;
			type_info[type_info_size++] = DECL_FUN;

			start_func_decl(id);

			parse_func_params();
		}
		else break;
	}

	for (int i = tmp_type_info_size - 1; i >= 0; --i) {
		type_info[type_info_size++] = tmp_type_info[i];
	}
}

// declarator : pointer? direct_declarator
void parse_declarator(int ctx) {
	// pointer
	while (token == T_MUL) {
		type_info[type_info_size++] = DECL_PTR;
		
		next_token();
	}

	char id[256];
	int func;

	parse_direct_declarator(id, &func);
	if (func) {
		next_token(); 
		if (token == T_LCURLY) {
			next_token();

			start_func_body();

			parse_stmts(T_RCURLY);
			end_func();
		}
		else {
			parse_token(T_SEMI);

			int id = get_global(funcname);
			global_vars[id][0] = ATTR_EXTERN;

			funcname[0] = 0;
		}

	}
	else {
		if (ctx == DECL_CTX_PARAM) {
			add_param(id);
		}
		else {
			if (funcname[0]) {
				add_local(id);
			}
			else {
				add_global(id);
			}
		}

		if (token == T_ASSIGNMENT && ctx != DECL_CTX_PARAM) {
			next_token();

			int index = get_local(id);
			int offset = local_vars[index][0];
			int size = get_type_byte_size(type_info, type_info_size);

			emit_push(VAL_LOCAL_ADDR, offset, size);

			parse_assignment_expr();

			emit(OP_SAVE);
			emit(size);
		}

		if (ctx == DECL_CTX_DEFAULT) {
			parse_token(T_SEMI);
		}
	}
}

void parse_declaration(int ctx) {
	parse_decl_specs();
	parse_declarator(ctx);
}

void parse_enum() {
	next_token();
	parse_token(T_LCURLY);

	int value = 0;

	char id[256];

	while (token != T_RCURLY && token != T_EOF) {
		strcpy(id, token_id);
		parse_token(T_ID);

		if (token == T_ASSIGNMENT) {
			next_token();

			value = token_num;
			parse_token(T_INT_LIT);
		}

		add_int_const(id, value++);

		if (token != T_COMMA) {
			break;
		}

		next_token();
	}

	parse_token(T_RCURLY);
	parse_token(T_SEMI);
}

void parse_stmt() {
	if (token == T_TYPEID || token == T_CONST) {
		parse_declaration(DECL_CTX_DEFAULT);
		return;
	}

	if (token == T_ENUM) {
		parse_enum();
		return;
	}

	if (token == T_RETURN) {
		next_token();

		if (token == T_SEMI) {
			emit_push(VAL_INT, 0, 0);
		}
		else {
			parse_assignment_expr();
		}

		parse_token(T_SEMI);

		emit(OP_RETURN);
		return;
	}

	if (token == T_IF) {
		next_token();

		parse_token(T_LPAREN);

		parse_assignment_expr();

		parse_token(T_RPAREN);

		int noteq_label = nlabels++;
		emit(OP_JMPZ);
		emit(noteq_label);

		parse_stmt();

		int end_label;
		if (token == T_ELSE) {
			end_label = nlabels++;
			emit(OP_JMP);
			emit(end_label);

			emit(OP_LABEL);
			emit(noteq_label);

			next_token();

			parse_stmt();
		} 
		else {
			end_label = noteq_label;
		}

		emit(OP_LABEL);
		emit(end_label);

		return;
	}

	if (token == T_SWITCH) {
		next_token();

		parse_token(T_LPAREN);

		set_int_type();
		int var = add_local("$tmp");
		int var_offs = local_vars[var][0];

		emit_push(VAL_LOCAL_ADDR, var_offs, 4);

		parse_assignment_expr();
		emit(OP_SAVE);
		emit(4);
		
		parse_token(T_RPAREN);

		int end_label = nlabels++;

		int scope = start_switch_scope(end_label, -1, var);

		parse_stmt();

		int last_case_label = scopes[scope][3];

		if (last_case_label != -1) {
			emit(OP_LABEL);
			emit(last_case_label);
		}

		emit(OP_LABEL);
		emit(end_label);
		end_scope();
		return;
	}

	if (token == T_CASE) {
		next_token();

		int scope = find_scope(SCOPE_SWITCH);
		int case_label = scopes[scope][3];
		int jmp_over_label = -1;

		if (case_label != -1) {
			// Jump over comparison for handling case falltrough
			jmp_over_label = nlabels++;
			emit(OP_JMP);
			emit(jmp_over_label);

			emit(OP_LABEL);
			emit(case_label);
		}

		parse_primary_expr();

		parse_token(T_COLON);
		
		int var = scopes[scope][4];
		int var_offs = local_vars[var][0];

		scopes[scope][3] = nlabels++;

		emit(OP_CASE);
		emit(var_offs); // var
		emit(scopes[scope][3]); // case_label

		if (jmp_over_label != -1) {
			emit(OP_LABEL);
			emit(jmp_over_label);
		}

		parse_stmt();
		return;
	}

	if (token == T_DEFAULT) {
		next_token();

		parse_token(T_COLON);

		int scope = find_scope(SCOPE_SWITCH);
		int case_label = scopes[scope][3];

		if (case_label != -1) {
			// Jump over comparison for handling case falltrough
			emit(OP_LABEL);
			emit(case_label);

			scopes[scope][3] = -1;
		}

		parse_stmt();
		return;
	}

	if (token == T_WHILE) {
		next_token();

		int start_label = nlabels++;
		int end_label = nlabels++;

		start_loop_scope(end_label, start_label);

		emit(OP_LABEL);
		emit(start_label);

		parse_token(T_LPAREN);

		parse_assignment_expr();

		parse_token(T_RPAREN);
		
		emit(OP_JMPZ);
		emit(end_label);

		parse_stmt();

		emit(OP_JMP);
		emit(start_label);

		emit(OP_LABEL);
		emit(end_label);

		end_scope();
		return;
	}

	if (token == T_DO) {
		next_token();

		int start_label = nlabels++;
		int end_label = nlabels++;
		int continue_label = nlabels++;

		start_loop_scope(end_label, continue_label);

		emit(OP_LABEL);
		emit(start_label);

		parse_stmt();

		parse_token(T_WHILE);
		parse_token(T_LPAREN);

		emit(OP_LABEL);
		emit(continue_label);

		parse_assignment_expr();

		parse_token(T_RPAREN);
		parse_token(T_SEMI);

		emit(OP_JMPZ);
		emit(end_label);

		emit(OP_JMP);
		emit(start_label);

		emit(OP_LABEL);
		emit(end_label);

		end_scope();

		return;
	}

	if (token == T_FOR) {
		next_token();

		int start_label = nlabels++;
		int end_label = nlabels++;
		int continue_label = nlabels++;

		start_loop_scope(end_label, continue_label);

		parse_token(T_LPAREN);

		if (token == T_TYPEID) {
			parse_declaration(DECL_CTX_FOR_INIT);
		}
		else if (token != T_SEMI) {
			parse_assignment_expr();
		}

		parse_token(T_SEMI);

		emit(OP_LABEL);
		emit(start_label);

		if (token != T_SEMI) {
			parse_assignment_expr();
		}

		emit(OP_JMPZ);
		emit(end_label);

		parse_token(T_SEMI);

		emit(OP_FOR);
		int increment_end = emit(0);
		int body_end = emit(0);

		emit(OP_LABEL);
		emit(continue_label);

		if (token != T_RPAREN) {
			parse_assignment_expr();
		}
		opcodes[increment_end] = nopcodes;

		parse_token(T_RPAREN);

		parse_stmt();

		opcodes[body_end] = nopcodes;

		emit(OP_JMP);
		emit(start_label);

		emit(OP_LABEL);
		emit(end_label);

		end_scope();
		return;
	}

	if (token == T_BREAK) {
		next_token();

		emit(OP_JMP);
		emit(find_break_label());

		parse_token(T_SEMI);
		return;
	}

	if (token == T_CONTINUE) {
		next_token();

		emit(OP_JMP);
		emit(find_continue_label());

		parse_token(T_SEMI);
		return;
	}

	// ;
	if (token == T_SEMI) {
		next_token();
		return;
	}

	//
	if (token == T_LCURLY) {
		start_block_scope();

		next_token();

		parse_stmts(T_RCURLY);

		end_scope();
		return;
	}

	parse_assignment_expr();
	parse_token(T_SEMI);
}

void parse_stmts(int terminator) {
	// prog = stmt* eof
	while (1) {
		if (token == terminator) {
			next_token();
			break;
		}

		if (token == T_EOF) {
			check_token(terminator);
			break;
		}

		parse_stmt();
	}
}

// end parser


// cpool
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


// end cpool

// codegen

int emit(int op) {
	opcodes[nopcodes++] = op;
	return nopcodes - 1;
}

void emit_push(int val_type, int val, int size) {
	emit(OP_PUSH);
	emit(val_type);
	emit(val);
	emit(size);
}

void start_func_decl(const char* s) {
	strcpy(funcname, s);

	int id = add_global(s);
	global_vars[id][1] = type_info_size;
	memcpy(&global_vars[id][2], type_info, type_info_size * sizeof(type_info[0]));

	nparams = 0;
}

void start_func_body() {
	nlocals = 0;
	nlabels = 0;
	stack_size = 0;

	start_block_scope();

	emit(OP_FUNC_PROLOGUE);
}

void end_func() {
	emit(OP_FUNC_EPILOGUE);

	gen_code(0, nopcodes);
	nopcodes = 0;
	end_scope();
}

int add_param(const char* s) {
	strcpy(params[nparams++], s);
	return nparams - 1;
}
int get_param(const char* s) {
	for (int i = 0; i < nparams; ++i) {
		if (!strcmp(params[i], s)) return i;
	}
	return -1;
}

int add_local(const char* s) {
	int index = nlocals++;
	strcpy(locals[index], s);
	stack_size += get_type_byte_size(type_info, type_info_size);

	local_vars[index][0] = -stack_size;
	local_vars[index][1] = type_info_size;
	memcpy(&local_vars[index][2], type_info, type_info_size * sizeof(type_info[0]));

	return index;
}

int get_local(const char* s) {
	for (int i = nlocals - 1; i >= 0; --i) {
		if (!strcmp(locals[i], s)) return i;
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


// end codegen

// asmgen

void gen_cpool() {
	fprintf(ftarget, ".const\n");

	for (int i = 0; i < cpool_size; ) {
		int size = strlen(&cpool[i]) + 1;
		fprintf(ftarget, "__str_%d db ", i);

		int quoted = 0;
		for (int j = 0; j < size; ++j) {
			char ch = cpool[i + j];
			if (ch < ' ' || ch == '\'') {
				if (quoted) fprintf(ftarget, "', ", ch);
				else if (j != 0) fprintf(ftarget, ", ");
				fprintf(ftarget, "%02XH", ch);
				quoted = 0;
			}
			else {
				if (j != 0 && !quoted) fprintf(ftarget, ", '");
				else if (!quoted) fprintf(ftarget, "'");
				fprintf(ftarget, "%c", ch);
				quoted = 1;
			}
		}

		fprintf(ftarget, "\n");

		i += size;
	}
	fprintf(ftarget, "\n");
}

const char* get_basic_type_asm_name(int size) {
	switch (size) {
	case 1:
		return "db";
	case 2:
		return "dw";
	case 4:
		return "dd";
	}
	return 0;
}

void gen_globals() {
	// public section
	for (int i = 0; i < nglobals; ++i) {
		if (global_vars[i][0] == ATTR_EXTERN) {
			fprintf(ftarget, "_%s PROTO\n", globals[i]);
		}
		else {
			fprintf(ftarget, "public _%s\n", globals[i]);
		}
	}

	fprintf(ftarget, ".data\n");
	for (int i = 0; i < nglobals; ++i) {
		if (global_vars[i][0] == ATTR_EXTERN) continue;

		int type_size = global_vars[i][1];
		if (global_vars[i][2 + type_size - 1] == DECL_FUN) continue;

		int type_info_size = global_vars[i][1];
		int* type_info = &global_vars[i][2];
		
		int basic_type = get_basic_type(type_info, type_info_size);
		const char* basic_type_name = get_basic_type_asm_name(types_sizes[basic_type]);

		int array_size = get_type_array_size(type_info, type_info_size);

		
		if (type_info[type_info_size - 1] == DECL_ARRAY) {
			fprintf(ftarget, "_%s %s %d dup (0)\n", globals[i], basic_type_name, array_size);
		}
		else {
			fprintf(ftarget, "_%s %s 0\n", globals[i], basic_type_name);
		}
	}
	fprintf(ftarget, "\n");
}

void emit_asm_cmp(const char* op) {
	fprintf(ftarget, "  pop eax\n");
	fprintf(ftarget, "  pop ecx\n");
	fprintf(ftarget, "  cmp ecx, eax\n");
	fprintf(ftarget, "  %s al\n", op);
	fprintf(ftarget, "  cbw\n");
	fprintf(ftarget, "  cwde\n");
	fprintf(ftarget, "  push eax\n");
}

void emit_asm_binop(const char* op) {
	fprintf(ftarget, "  pop eax\n");
	fprintf(ftarget, "  pop ecx\n");
	fprintf(ftarget, "  %s ecx, eax\n", op);
	fprintf(ftarget, "  push ecx\n");
}

const char* get_asm_type(int size) {
	switch (size) {
	case 1:
		return "BYTE";

	case 2:
		return "WORD";

	case 4:
		return "DWORD";

	default:
		return "XXX";
	}
}

const char* get_asm_reg(int size) {
	switch (size) {
	case 1:
		return "al";

	case 2:
		return "ax";

	case 4:
		return "eax";

	default:
		return "xxx";
	}
}

void gen_move(const char* dst, const char* src, int offset, int size) {
	const char* instr = size < 4 ? "movsx" : "mov";

	fprintf(ftarget, "  %s %s, %s PTR [%s]%+d\n", instr, dst, get_asm_type(size), src, offset);
}

void gen_code(int from, int end) {
	for (int i = from; i < end; ) {
		int op = opcodes[i++];
		if (op == OP_FUNC_PROLOGUE) {
			fprintf(ftarget, "_%s proc\n", funcname);
			fprintf(ftarget, "  push ebp\n");
			fprintf(ftarget, "  mov ebp, esp\n");
			fprintf(ftarget, "  sub esp, %d\n\n", stack_size);
		}
		else if (op == OP_FUNC_EPILOGUE) {
			fprintf(ftarget, "\n__exit_%s:\n", funcname);
			fprintf(ftarget, "  mov esp, ebp\n");
			fprintf(ftarget, "  pop ebp\n");
			fprintf(ftarget, "  ret 0\n");
			fprintf(ftarget, "_%s endp\n\n", funcname);
		}
		else if (op == OP_PUSH) {
			int value_type = opcodes[i++];
			int value = opcodes[i++];
			int size = opcodes[i++];

			if (value_type == VAL_STR) {
				fprintf(ftarget, "  push OFFSET __str_%d\n", value);
			}
			else if (value_type == VAL_INT) {
				fprintf(ftarget, "  push %d\n", value);
			}
			else if (value_type == VAL_LOCAL) {
				if (size == 0) {
					size = 4;
				}

				gen_move("eax", "ebp", value, size);
				fprintf(ftarget, "  push eax\n", value);
			}
			else if (value_type == VAL_LOCAL_ADDR) {
				if (value >= 0) {
					fprintf(ftarget, "  lea eax, DWORD PTR [ebp]+%d \n", value);
				}
				else {
					fprintf(ftarget, "  lea eax, DWORD PTR [ebp]%d \n", value);
				}

				fprintf(ftarget, "  push eax\n", value);
			}
			else if (value_type == VAL_GLOB) {
				fprintf(ftarget, "  push DWORD PTR _%s\n", globals[value]);
			}
			else if (value_type == VAL_GLOB_ADDR) {
				fprintf(ftarget, "  push OFFSET _%s\n", globals[value]);
			}
		}
		else if (op == OP_DEREF) {
			int item_size = opcodes[i++];
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  imul ecx, %d\n", item_size);
			gen_move("eax", "eax + ecx", 0, item_size);

			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_DEREF_ADDR) {
			int item_size = opcodes[i++];
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  imul ecx, %d\n", item_size);
			fprintf(ftarget, "  lea eax, DWORD PTR [eax + ecx]\n");
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_DUP_VALUE) {
			int size = opcodes[i++];
			fprintf(ftarget, "  mov eax, DWORD PTR [esp]\n"); // peek addr from the top of the stack
			gen_move("eax", "eax", 0, size); // dereference addr
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_SAVE) {
			int size = opcodes[i++];

			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  pop ecx\n");

			const char* reg = get_asm_reg(size);
			fprintf(ftarget, "  mov %s PTR [ecx], %s\n", get_asm_type(size), reg);
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_CALL) {
			int header_ref = opcodes[i];
			int param_count = opcodes[header_ref];

			i = header_ref + param_count + 1;

			int paramEnd = header_ref;
			for (int param = param_count - 1; param >= 0; --param) {
				int paramOffset = opcodes[header_ref + param + 1];
				gen_code(paramOffset, paramEnd);

				paramEnd = paramOffset;
			}

			// FIXME: support different parameter sizes
			int st = param_count * 4;
			fprintf(ftarget, "  call DWORD PTR [esp+%d]\n", st);
			fprintf(ftarget, "  add esp, %d\n", st+4); // size of paremters + pointer to function
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_RETURN) {
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  jmp __exit_%s\n", funcname);
		}
		else if (op == OP_FOR) {
			// post increment instructions after body

			int increment_end = opcodes[i++];
			int body_end = opcodes[i++];
			gen_code(increment_end, body_end);
			gen_code(i, increment_end);
			
			i = body_end;
		}
		else if (op == OP_JMP) {
			int label = opcodes[i++];
			fprintf(ftarget, "  jmp __%s_%d\n", funcname, label);
		}
		else if (op == OP_JMPZ) {
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  test eax, eax\n");

			int label = opcodes[i++];
			fprintf(ftarget, "  je __%s_%d\n", funcname, label);
		}
		else if (op == OP_JMPNZ) {
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  test eax, eax\n");

			int label = opcodes[i++];
			fprintf(ftarget, "  jne __%s_%d\n", funcname, label);
		}
		else if (op == OP_CASE) {
			int var = opcodes[i++];
			int label = opcodes[i++];
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  cmp DWORD PTR [ebp]%+d, eax\n", var);
			fprintf(ftarget, "  jne __%s_%d\n", funcname, label);
		}
		else if (op == OP_LABEL) {
			int id = opcodes[i++];
			fprintf(ftarget, "\n__%s_%d:\n", funcname, id);
		}
		else if (op == OP_ADD) {
			emit_asm_binop("add");
		}
		else if (op == OP_SUB) {
			emit_asm_binop("sub");
		}
		else if (op == OP_MUL) {
			emit_asm_binop("imul");
		}
		else if (op == OP_DIV) {
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  cdq\n");
			fprintf(ftarget, "  idiv ecx\n", op);
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_MOD) {
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  cdq\n");
			fprintf(ftarget, "  idiv ecx\n", op);
			fprintf(ftarget, "  push edx\n");
		}
		else if (op == OP_INC || op == OP_DEC) {
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  %s DWORD PTR [eax], 1\n", op == OP_INC ? "add" : "sub");
			fprintf(ftarget, "  mov eax, DWORD PTR [eax]\n");
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_INC_POST || op == OP_DEC_POST) {
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  mov eax, DWORD PTR [ecx]\n");
			fprintf(ftarget, "  %s DWORD PTR  [ecx], 1\n", op == OP_INC_POST ? "add" : "sub");
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_LESS) {
			emit_asm_cmp("setl");
		}
		else if (op == OP_GREATER) {
			emit_asm_cmp("setg");
		}
		else if (op == OP_LESS_EQ) {
			emit_asm_cmp("setle");
		}
		else if (op == OP_GREATER_EQ) {
			emit_asm_cmp("setge");
		}
		else if (op == OP_EQ) {
			emit_asm_cmp("sete");
		}
		else if (op == OP_NOT_EQ) {
			emit_asm_cmp("setne");
		}
		else if (op == OP_NOT){
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  xor eax, eax\n");
			fprintf(ftarget, "  cmp ecx, eax\n");
			fprintf(ftarget, "  sete al\n");
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_NEG){
			fprintf(ftarget, "  neg DWORD PTR [esp]\n");
		}
	}
}

void gen_asm() {
	// header
	fprintf(ftarget, ".586\n");
	fprintf(ftarget, ".model flat\n\n");
	fprintf(ftarget, "includelib msvcrtd\n");

	fprintf(ftarget, ".code\n\n");

	lineno = 1;
	next_token();
	parse_stmts(T_EOF);

	gen_globals();
	gen_cpool();

	fprintf(ftarget, "end\n");
	
}
// 

void dump_type(int* type_info, int size) {
	int array_size = get_type_array_size(type_info, size);
	int basic_type = get_basic_type(type_info, size);
	printf("TYPE basic_type: %s, size %d\n", types[basic_type], array_size);

	for (int i = size - 1; i >= 0; --i) {
		switch (type_info[i]) {
		case DECL_ARRAY:
			fprintf(ftarget, "[%d] of ", type_info[--i]);
			break;

		case DECL_PTR:
			fprintf(ftarget, "ptr to ");
			break;

		case DECL_FUN:
			fprintf(ftarget, "function returning ");
			break;

		case DECL_BASIC:
			fprintf(ftarget, "%s\n", types[type_info[--i]]);
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

int start_scope(int type) {
	if (nscopes > 0)  {
		scopes[nscopes - 1][1] = nlocals;
	}
	scopes[nscopes][0] = type;
	scopes[nscopes][1] = 0;
	return nscopes++;
}

int start_loop_scope(int break_label, int continue_label) {
	int i = start_scope(SCOPE_LOOP);
	scopes[i][2] = break_label;
	scopes[i][3] = continue_label;
	return i;
}

int start_block_scope() {
	return start_scope(SCOPE_BLOCK);
}

int start_switch_scope(int break_label, int case_label, int var) {
	int i = start_scope(SCOPE_SWITCH);
	scopes[i][2] = break_label;
	scopes[i][3] = case_label;
	scopes[i][4] = var;
	return i;
}

void end_scope() {
	--nscopes;
	
	if (nscopes > 0) {
		nlocals = scopes[nscopes - 1][1];
	}
	else {
		nlocals = 0;
	}
}

int find_scope(int type) {
	for (int i = nscopes - 1; i >= 0; --i) {
		if (scopes[i][0] == type) return i;
	}
	return -1;
}

int find_break_label() {
	for (int i = nscopes - 1; i >= 0; --i) {
		if (scopes[i][0] == SCOPE_LOOP || scopes[i][0] == SCOPE_SWITCH) return scopes[i][2];
	}

	return -1;
}
int find_continue_label() {
	for (int i = nscopes - 1; i >= 0; --i) {
		if (scopes[i][0] == SCOPE_LOOP) return scopes[i][3];
	}

	return -1;
}

void add_extern_func(const char* s) {
	type_info_size = 0;
	type_info[type_info_size++] = TYPE_INT;
	type_info[type_info_size++] = DECL_BASIC;
	type_info[type_info_size++] = DECL_FUN;

	int id = add_global(s);
	global_vars[id][0] = ATTR_EXTERN;
}

int add_type(const char* s, int size) {
	int id = ntypes++;
	types[id] = s;
	types_sizes[id] = size;

	return id;
}

int main(int argc, char** argv) {

	if (argc != 3)  {
		printf("Usage ccomp <source.c> <target>\n");
		return -1;
	}

	add_int_const("EOF", -1);

	add_extern_func("fopen");
	add_extern_func("fclose");
	add_extern_func("fgetc");
	add_extern_func("ungetc");
	add_extern_func("fprintf");
	add_extern_func("printf");
	add_extern_func("exit");
	add_extern_func("strcpy");
	add_extern_func("strcmp");
	add_extern_func("strlen");
	add_extern_func("memcpy");

	add_type("void", 0);
	add_type("char", 1);
	add_type("int", 4);
	add_type("FILE", 4);

	fsource = fopen(argv[1], "r");
	ftarget = fopen(argv[2], "w");
	gen_asm();

	fclose(fsource);
	fclose(ftarget);

}