#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cstring>

// expressions: assignment, ident, plus, call function, nums
// 

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

	, T_ASSIGNMENT
	, T_EQ

	, T_LPAREN // (
	, T_RPAREN // )
	, T_LBRACKET // [
	, T_RBRACKET // ]
	, T_LCURLY // {
	, T_RCURLY // {

	// keywords
	, T_RETURN
	, T_IF
};


FILE* f;
int token;
char token_id[256];
int token_num;

int ntypes = 1;
char* types[256] = {
	"int"
};

// cpool
int cpool_size = 0;
char cpool[1024];

int add_const(const char*);

// end cpool

// codegen
enum {
	OP_LOAD
	, OP_ADDR
	, OP_PUSH
	, OP_SAVE
	, OP_CALL
};
enum {
	VAL_STR,
	VAL_INT,
	VAL_LOCAL,
	VAL_LOCAL_ADDR,
	VAL_EXTERN
};

int nopcodes = 0;
int opcodes[1024];

int emit(int);

int nparams;
char params[256][64];

int nlocals;
char locals[256][64];

int nexterns;
char externs[256][24];

int add_extern(const char* s);

int add_param(const char* s);
int add_local(const char* s);
int get_local(const char* s);
int get_param(const char* s);
int get_extern(const char* s);

void start_func(const char* s);
void end_func(const char* s);
// end

// asmgen
void gen_func_prologue(const char* name);
void gen_func_epilogue(const char* name);
void gen_code();
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

	case T_ASSIGNMENT: return "ASSIGNMENT";
	case T_EQ: return "EQ";

	case T_LPAREN: return "LPAREN";
	case T_RPAREN: return "RPAREN";
	case T_LBRACKET: return "LBRACKET";
	case T_RBRACKET: return "RBRACKET";
	case T_LCURLY: return "LCURLY";
	case T_RCURLY: return "RCURLY";

	case T_RETURN: return "RETURN";
	case T_IF: return "IF";
	}
	return "XXX";
}


int next_token_helper() {
	int ch;

	do {
		ch = fgetc(f);
	} while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

	token_id[0] = 0;

	if (ch == EOF) return T_EOF;
	if (ch == ';') return T_SEMI;
	if (ch == ',') return T_COMMA;
	if (ch == '.') return T_PERIOD;
	if (ch == ':') return T_COLON;

	if (ch == '=') return T_ASSIGNMENT;

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
			ch = fgetc(f);
		} while (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == '_');

		token_id[i] = 0;

		int t = T_ID;
		for (int i = 0; i < ntypes; ++i) {
			if (strcmp(types[i], token_id) == 0) {
				t = T_TYPEID;
				break;
			}
		}

		ungetc(ch, f);
		return t;
	}

	if (ch >= '0' && ch <= '9') {
		int n = 0;
		do {
			n = n * 10 + (ch - '0');
			ch = fgetc(f);
		} while (ch >= '0' && ch <= '9');

		token_num = n;
		ungetc(ch, f);
		return T_INT_LIT;
	}

	if (ch == '"') {
		int i = 0;
		ch = fgetc(f);
		while (ch != '"' && ch != EOF) {
			token_id[i++] = ch;
			ch = fgetc(f);
		}

		token_id[i] = 0;
		return T_STR_LIT;
	}

	printf("Invalid token %c\n", ch);
	return T_EOF;
}

void next_token() {
	token = next_token_helper();
}

// end scanner

// parser
void parse_assignment_expr();
void parse_stmts(int);


bool check_token(int expected) {
	if (token != expected) {
		printf("Syntax error: unexpected token %s, expected %s\n", tok2str(token), tok2str(expected));
		return false;
	}
	return true;
}

void parse_token(int expected) {
	next_token();
	check_token(expected);
}

bool try_parse_token(int expected) {
	next_token();
	return token == expected;
}

void parse_primary_expr() {
	if (token == T_INT_LIT) {
		emit(OP_PUSH); 
		emit(VAL_INT);
		emit(token_num);

		next_token();
		return;
	}
	if (token == T_STR_LIT) {
		int cid = add_const(token_id);
		emit(OP_PUSH);
		emit(VAL_STR);
		emit(cid);

		next_token();
		return;
	}
	if (check_token(T_ID)) {
		int id = get_local(token_id);
		if (id != -1) {
			emit(OP_PUSH);
			emit(VAL_LOCAL);
			emit(-4 * (id + 1));
		}
		else if ((id = get_param(token_id)) != -1) {
			emit(OP_PUSH);
			emit(VAL_LOCAL);
			emit(4 * (id + 2));
		}
		else if ((id = get_extern(token_id)) != -1) {
			emit(OP_PUSH);
			emit(VAL_EXTERN);
			emit(id);
		}
		else {
			printf("Unknown identifier %s\n", token_id);
		}
		next_token();
		return;
	}
}

void parse_postfix_expr() {
	parse_primary_expr();

	if (token == T_LPAREN) {
		//printf("(");
		next_token();
		parse_assignment_expr();

		//printf(")");
		check_token(T_RPAREN);
		next_token();

		emit(OP_CALL);
		emit(4); // size of the pushed parameters
	}

}

void parse_assignment_expr() {
	int lhs = nopcodes;
	parse_postfix_expr();

	if (token == T_ASSIGNMENT) {
		next_token(); 
		parse_assignment_expr();

		if (opcodes[lhs] == OP_PUSH && opcodes[lhs + 1] == VAL_LOCAL) {
			opcodes[lhs + 1] = VAL_LOCAL_ADDR;
		}

		emit(OP_SAVE);
	}
}

void parse_func_params() {
	next_token();
	while (token != T_RPAREN && token != T_EOF) {

		check_token(T_TYPEID);
		parse_token(T_ID);
		add_param(token_id);

		if (!try_parse_token(T_COMMA)) {
			break;
		}

		next_token();
	}

	check_token(T_RPAREN);
}


void parse_stmt() {
	// func_decl: TYPEID ID () {}
	// var_decl: TYPEID ID;
	if (token == T_TYPEID) {
		char type[256], id[256];

		strcpy(type, token_id);

		parse_token(T_ID);
		strcpy(id, token_id);

		if (try_parse_token(T_LPAREN)) {
			start_func(id);

			parse_func_params();

			parse_token(T_LCURLY);
			

			parse_stmts(T_RCURLY);
			end_func(id);
		}
		else {
			add_local(id);
			check_token(T_SEMI);
		}
		return;
	}

	// ;
	if (token == T_SEMI) return;

	parse_assignment_expr();
	//printf(";\n");
	check_token(T_SEMI);
}

void parse_stmts(int terminator) {
	// prog = stmt* eof
	while (1) {
		next_token();
		if (token == terminator) break;

		parse_stmt();
	}
}

void parse_stmts() {
	// prog = stmt* eof
	while (1) {
		next_token();
		if (token == T_RCURLY) break;

		parse_stmt();
	}
}

// end parser


// cpool
int add_const(const char* s) {
	int size = strlen(s) + 1;

	memcpy(cpool + cpool_size, s, size);
	cpool_size += size;
	return cpool_size - size;
}

// end cpool

// codegen

int emit(int op) {
	opcodes[nopcodes++] = op;
	return nopcodes - 1;
}

void start_func(const char* s) {
	gen_func_prologue(s);
	nparams = 0;
	nlocals = 0;
}

void end_func(const char* s) {
	gen_code();
	nopcodes = 0;

	gen_func_epilogue(s);
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
	strcpy(locals[nlocals++], s);
	return nlocals - 1;
}

int get_local(const char* s) {
	for (int i = 0; i < nlocals; ++i) {
		if (!strcmp(locals[i], s)) return i;
	}
	return -1;
}

int add_extern(const char* s) {
	strcpy(externs[nexterns++], s);
	return nexterns - 1;
}

int get_extern(const char* s) {
	for (int i = 0; i < nexterns; ++i) {
		if (!strcmp(externs[i], s)) return i;
	}
	return -1;
}


// end codegen

// asmgen
void gen_func_prologue(const char* name) {
	printf("%s proc\n", name);
	printf("  push ebp\n");
	printf("  mov ebp, esp\n");
	printf("  sub esp, 72\n\n"); // FIXME

}

void gen_func_epilogue(const char* name) {
	printf("\n");
	printf("  mov esp, ebp\n");
	printf("  pop ebp\n");
	printf("  ret 0\n");

	printf("%s endp\n\n", name);
}

void gen_cpool() {
	printf(".const\n");

	for (int i = 0; i < cpool_size; ) {
		int size = strlen(&cpool[i]) + 1;
		printf("__str_%d db '%s', 0h\n", i, &cpool[i]);

		i += size;
	}
	printf("\n");
}

void gen_code() {
	for (int i = 0; i < nopcodes; ) {
		int op = opcodes[i++];
		if (op == OP_PUSH) {
			int valueType = opcodes[i++];
			int value = opcodes[i++];

			if (valueType == VAL_STR) {
				printf("  push OFFSET __str_%d\n", value);
			}
			else if (valueType == VAL_INT) {
				printf("  push %d\n", value);
			}
			else if (valueType == VAL_LOCAL) {
				if (value >= 0) {
					printf("  mov eax, DWORD PTR [ebp]+%d \n", value);
				}
				else {
					printf("  mov eax, DWORD PTR [ebp]%d \n", value);
				}
				printf("  push eax \n", value);
			}
			else if (valueType == VAL_LOCAL_ADDR) {
				if (value >= 0) {
					printf("  lea eax, DWORD PTR [ebp]+%d \n", value);
				}
				else {
					printf("  lea eax, DWORD PTR [ebp]%d \n", value);
				}

				printf("  push eax \n", value);
			}
			else if (valueType == VAL_EXTERN) {
				printf("  push OFFSET %s\n", externs[value]);
			}
		}
		else if (op == OP_SAVE) {
			printf("  pop eax\n");
			printf("  pop ecx\n");
			printf("  mov  DWORD PTR [ecx], eax\n");
		}
		else if (op == OP_CALL) {
			int st = opcodes[i++];
			printf("  call DWORD PTR [esp+%d]\n", st);
		}
	}
}

void gen_asm() {
	// header
	printf(".586\n");
	printf(".model flat, c\n\n");

	printf(".code\n\n");

	parse_stmts(T_EOF);

	gen_cpool();

	printf("end\n");
	
}
// 

int main(int argc, char** argv) {
	if (argc != 2) return -1;


	add_extern("printf");
	add_extern("exit");

	f = fopen(argv[1], "r");
	gen_asm();

	fclose(f);

}