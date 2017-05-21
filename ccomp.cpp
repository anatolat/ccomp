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
	, T_ASSIGNMENT_ADD
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
};

FILE* f;
int token;
char token_id[256];
int token_num;
int lineno;

int ntypes = 3;
enum {
	TYPE_VOID,
	TYPE_CHAR,
	TYPE_INT
};

char* types[256] = {
	"void",
	"char",
	"int",
};
int types_sizes[256] = {
	0,
	1,
	4,
};

// cpool
int cpool_size = 0;
char cpool[1024];

int add_const(const char*);

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
	, OP_FOR
	, OP_INC
	, OP_DEC
	, OP_INC_POST
	, OP_DEC_POST
	, OP_NOT
};

enum {
	VAL_STR,
	VAL_INT,
	VAL_LOCAL,
	VAL_LOCAL_ADDR,
	VAL_GLOB,
	VAL_GLOB_ADDR,
	VAL_EXTERN
};


enum {
	DECL_ARRAY = 1,
	DECL_PTR,
	DECL_BASIC
};


int nopcodes = 0;
int opcodes[1024];

int emit(int);
void emit_push(int val_type, int val, int size);

char funcname[256];

int nparams;
char params[256][64];

int nlocals;
int stack_size;
char locals[256][64];
int local_vars[256][64];

int nlabels;

int nexterns;
char externs[256][64];

int nglobals;
char globals[256][64];
int global_vars[256][64];

int last_value_ref = -1;

int type_info_size;
int type_info[63];
	
void dump_type(int* type_info, int size);
int get_basic_type(int* type_info, int size);
int get_type_array_size(int* type_info, int size);
int get_type_byte_size(int* type_info, int size);
void get_item_type_info(int* dest_type_info, int* dest_size, int* type_info, int type_info_size);
void set_type_placeholder();

int add_extern(const char* s);
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

	case T_ASSIGNMENT: return "ASSIGNMENT";
	case T_ASSIGNMENT_ADD: return "ASSIGNMENT_ADD";
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
	}
	return "XXX";
}


int next_token_helper() {
	int ch;

	do {
		ch = fgetc(f);
		if (ch == '\n') ++lineno;


		// ignore preprocessor directives
		if (ch == '#') {
			while (ch != '\n' && ch != EOF) {
				ch = fgetc(f);
			}

			if (ch == '\n') ++lineno;
		}
		// comments
		else if (ch == '/') {
			ch = fgetc(f);
			if (ch == '/') {
				while (ch != '\n' && ch != EOF) {
					ch = fgetc(f);
				}

				if (ch == '\n') ++lineno;
			}
			else if (ch == '*') {
				while (ch != EOF) {
					ch = fgetc(f);
					if (ch == '\n') ++lineno;

					if (ch == '*') {
						ch = fgetc(f);
						if (ch == '/') {
							ch = fgetc(f);
							break;
						}

						ungetc(ch, f);
					}
				}
			}
			else {
				ungetc(ch, f);
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

	if (ch == '=') {
		ch = fgetc(f);
		if (ch == '=') return T_EQ;

		ungetc(ch, f);
		return T_ASSIGNMENT;
	}
	if (ch == '+') {
		ch = fgetc(f);
		if (ch == '+') return T_INC;
		if (ch == '=') return T_ASSIGNMENT_ADD;

		ungetc(ch, f);
		return T_ADD;
	}
	if (ch == '-') {
		ch = fgetc(f);
		if (ch == '-') return T_DEC;

		ungetc(ch, f);
		return T_SUB;
	}
	if (ch == '*') return T_MUL;
	if (ch == '%') return T_MOD;
	if (ch == '/') return T_DIV;
	if (ch == '<') {
		ch = fgetc(f);
		if (ch == '=') return T_LESS_EQ;

		ungetc(ch, f);
		return T_LESS;
	}
	if (ch == '>') {
		ch = fgetc(f);
		if (ch == '=') return T_GREATER_EQ;

		ungetc(ch, f);
		return T_GREATER;
	}

	if (ch == '|') {
		ch = fgetc(f);
		if (ch == '|') return T_OR;

		ungetc(ch, f);
		return T_BIT_OR;
	}

	if (ch == '&') {
		ch = fgetc(f);
		if (ch == '&') return T_AND;

		ungetc(ch, f);
		return T_BIT_AND;
	}

	if (ch == '!') {
		ch = fgetc(f);
		if (ch == '=') return T_NOT_EQ;

		ungetc(ch, f);
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
			ch = fgetc(f);
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
		else {
			for (int i = 0; i < ntypes; ++i) {
				if (!strcmp(types[i], token_id)) {
					t = T_TYPEID;
					token_num = i;
					break;
				}
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
			if (ch == '\\') {
				ch = fgetc(f);

				if (ch == EOF) continue;

				if (ch == 'n') ch = '\n';
				else if (ch == 'r') ch = '\r';
				else if (ch == 't') ch = '\t';
			}

			token_id[i++] = ch;
			ch = fgetc(f);
		}

		token_id[i] = 0;
		return T_STR_LIT;
	}

	if (ch == '\'') {
		// TODO
		ch = fgetc(f);
		if (ch == '\\') {
			ch = fgetc(f);

			if (ch == 'n') ch = '\n';
			else if (ch == 'r') ch = '\r';
			else if (ch == 't') ch = '\t';
		}
		token_num = ch;

		fgetc(f);
		return T_CHAR_LIT;
	}


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
void parse_stmts(int);
void parse_declaration(int);


bool check_token(int expected) {
	if (token != expected) {
		printf("Syntax error (%d): unexpected token %s, expected %s\n", lineno, tok2str(token), tok2str(expected));
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
		int cid = add_const(token_id);
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

		check_token(T_RPAREN);
		next_token();
		return;
	}
	if (check_token(T_ID)) {
		int id = get_local(token_id);
		if (id != -1) {
			int offs = local_vars[id][0];

			type_info_size = local_vars[id][1];
			memcpy(type_info, &local_vars[id][2], type_info_size * sizeof(type_info[0]));

			bool is_addr = type_info[type_info_size - 1] == DECL_ARRAY;
			int size = get_type_byte_size(type_info, type_info_size);

			emit_push(is_addr ? VAL_LOCAL_ADDR : VAL_LOCAL, offs, size);
		}
		else if ((id = get_param(token_id)) != -1) {
			emit_push(VAL_LOCAL, 4 * (id + 2), 0);

			set_type_placeholder();
		}
		else if ((id = get_extern(token_id)) != -1) {
			emit_push(VAL_EXTERN, id, 0);

			set_type_placeholder();
		}
		else if ((id = get_global(token_id)) != -1) {
			type_info_size = global_vars[id][0];
			memcpy(type_info, &global_vars[id][1], type_info_size * sizeof(type_info[0]));

			bool is_addr = type_info[type_info_size - 1] == DECL_ARRAY;
			emit_push(is_addr ? VAL_GLOB_ADDR : VAL_GLOB, id, 0);
		}
		else {
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
			int sizes[256] = {}; // size of the code generated for each parameter
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

			check_token(T_RPAREN);
			next_token();

			opcodes[call_header] = emit(param_count);
			for (int i = 0; i < param_count; ++i) {
				emit(sizes[i]);
			}

			set_type_placeholder();
		}
		else if (token == T_LBRACKET) {
			next_token();

			int item_type_info[64];
			int item_type_info_size;

			//dump_type(type_info, type_info_size);
			get_item_type_info(item_type_info, &item_type_info_size, type_info, type_info_size);

			convert_to_addr(last_value_ref);

			parse_assignment_expr();

			bool is_addr = item_type_info[item_type_info_size - 1] == DECL_ARRAY;
			last_value_ref = nopcodes;

			emit(is_addr ? OP_DEREF_ADDR : OP_DEREF);
			emit(get_type_byte_size(item_type_info, item_type_info_size));

			check_token(T_RBRACKET);
			next_token();

			type_info_size = item_type_info_size;
			memcpy(type_info, item_type_info, item_type_info_size * sizeof(type_info[0]));
		}
		else if (token == T_INC || token == T_DEC) {
			bool inc = token == T_INC;
			next_token();

			convert_to_addr(last_value_ref);

			last_value_ref = nopcodes;
			emit(inc ? OP_INC_POST : OP_DEC_POST);

			set_type_placeholder();
			break;
		}
		else break;
	}
}

void parse_unary_expr() {
	if (token == T_INC || token == T_DEC) {
		bool inc = token == T_INC;
		next_token();
		
		parse_unary_expr();

		convert_to_addr(last_value_ref);
		last_value_ref = nopcodes;

		emit(inc ? OP_INC : OP_DEC);

		set_type_placeholder();
		return;
	}
	if (token == T_NOT) {
		next_token();

		parse_unary_expr();

		last_value_ref = nopcodes;
		emit(OP_NOT);

		set_type_placeholder();
		return;
	}
	
	parse_postfix_expr();
}

int op_prec(int op) {
	switch (op) {
	case T_MUL: case T_DIV: case T_MOD:
		return 11;

	case T_ADD: case T_SUB: 
		return 10;

	case T_SHL: case T_SHR:
		return 9;

	case T_LESS: case T_GREATER:
	case T_LESS_EQ: case T_GREATER_EQ:
		return 8;	

	case T_EQ: case T_NOT_EQ:
		return 7;

	case T_BIT_AND:
		return 6;

	case T_BIT_XOR:
		return 5;

	case T_BIT_OR:
		return 4;

	case T_AND:
		return 3;

	case T_OR:
		return 2;

	case T_ASSIGNMENT:
	case T_ASSIGNMENT_ADD:
		return 1;

	default:
		return -1;
	}
}

void emit_or_jump(int label) {
	emit(OP_JMPNZ);
	emit(label);
}

void emit_and_jump(int label) {
	emit(OP_JMPZ);
	emit(label);
}

void parse_expr(int min_prec) {
	parse_unary_expr();

	int or_label = -1;
	int and_label = -1;

	while (true) {
		int prec = op_prec(token);
		if (prec < min_prec) break;

		int op = token;
		next_token();

		if (op == T_OR) {
			or_label = nlabels++;
			emit_or_jump(or_label);
		}
		else if (op == T_AND) {
			and_label = nlabels++;
			emit_and_jump(and_label);
		}

		int lvalue_ref = last_value_ref;
		int lvalue_size = get_type_byte_size(type_info, type_info_size);

		parse_expr(prec == 1 ? prec : prec + 1);

		if (op == T_ASSIGNMENT) {
			convert_to_addr(lvalue_ref);
			emit(OP_SAVE);
			emit(lvalue_size);
		}
		else if (op == T_OR || op == T_AND) {
			// do nothing
		}
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
			printf("Internal error (%d): operator not supported %s\n", lineno, tok2str(op));
		}
	}

	if (or_label != -1) {
		emit_or_jump(or_label);
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

	if (and_label != -1) {
		emit_and_jump(and_label);
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
	check_token(T_TYPEID);
	
	type_info_size = 0;
	type_info[type_info_size++] = token_num;
	type_info[type_info_size++] = DECL_BASIC;

}

// declaration_specifiers
void parse_decl_specs() {
	parse_decl_spec();

	next_token();
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
void parse_direct_declarator(char* id, bool* func) {
	*func = false;
	if (!check_token(T_ID)) return;

	strcpy(id, token_id);
	next_token();
	
	int tmp_type_info_size = 0;
	int tmp_type_info[63];

	while (true) {
		if (token == T_LBRACKET) {
			next_token();

			check_token(T_INT_LIT);
			tmp_type_info[tmp_type_info_size++] = DECL_ARRAY;
			tmp_type_info[tmp_type_info_size++] = token_num;

			next_token();

			check_token(T_RBRACKET);
			next_token();
		}
		else if (token == T_LPAREN) {
			*func = true;
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
	bool func;

	parse_direct_declarator(id, &func);
	if (func) {
		if (try_parse_token(T_LCURLY)) {
			next_token();

			start_func_body();

			parse_stmts(T_RCURLY);
			end_func();
		}
		else {
			check_token(T_SEMI);
			next_token();
		}

	}
	else {
		if (ctx == DECL_CTX_PARAM) {
			add_param(id);
		}
		else {
			if (funcname[0]) {
				int index = add_local(id);
				stack_size += get_type_byte_size(type_info, type_info_size);

				local_vars[index][0] = -stack_size;
				local_vars[index][1] = type_info_size;
				memcpy(&local_vars[index][2], type_info, type_info_size * sizeof(type_info[0]));
			}
			else {
				int index = add_global(id);

				global_vars[index][0] = type_info_size;
				memcpy(&global_vars[index][1], type_info, type_info_size * sizeof(type_info[0]));
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
			check_token(T_SEMI);
			next_token();
		}
	}
}

void parse_declaration(int ctx) {
	parse_decl_specs();
	parse_declarator(ctx);

	//dump_type(type_info, type_info_size);
}

void parse_stmt() {
	// func_decl: TYPEID ID () {}
	// var_decl: TYPEID ID;
	if (token == T_TYPEID) {
		parse_declaration(DECL_CTX_DEFAULT);
		return;
	}

	if (token == T_RETURN) {
		next_token();
		parse_assignment_expr();
		check_token(T_SEMI);
		next_token();

		emit(OP_RETURN);
		return;
	}

	if (token == T_IF) {
		parse_token(T_LPAREN);

		next_token();
		parse_assignment_expr();

		check_token(T_RPAREN);

		int noteqLabel = nlabels++;
		emit(OP_JMPZ);
		emit(noteqLabel);

		next_token();
		parse_stmt();

		int endLabel;
		if (token == T_ELSE) {
			endLabel = nlabels++;
			emit(OP_JMP);
			emit(endLabel);

			emit(OP_LABEL);
			emit(noteqLabel);

			next_token();

			parse_stmt();
		} 
		else {
			endLabel = noteqLabel;
		}

		emit(OP_LABEL);
		emit(endLabel);

		return;
	}

	if (token == T_WHILE) {
		int startLabel = nlabels++;
		int endLabel = nlabels++;
		emit(OP_LABEL);
		emit(startLabel);

		parse_token(T_LPAREN);

		next_token();
		parse_assignment_expr();

		check_token(T_RPAREN);
		next_token();
		
		emit(OP_JMPZ);
		emit(endLabel);

		parse_stmt();

		emit(OP_JMP);
		emit(startLabel);

		emit(OP_LABEL);
		emit(endLabel);

		return;
	}

	if (token == T_DO) {
		next_token();

		int startLabel = nlabels++;
		int endLabel = nlabels++;
		emit(OP_LABEL);
		emit(startLabel);

		parse_stmt();

		check_token(T_WHILE);
		next_token();

		check_token(T_LPAREN);
		next_token();

		parse_assignment_expr();

		check_token(T_RPAREN);
		next_token();

		check_token(T_SEMI);
		next_token();

		emit(OP_JMPZ);
		emit(endLabel);

		emit(OP_JMP);
		emit(startLabel);

		emit(OP_LABEL);
		emit(endLabel);

		return;
	}

	if (token == T_FOR) {
		int startLabel = nlabels++;
		int endLabel = nlabels++;

		parse_token(T_LPAREN);
		next_token();

		if (token == T_TYPEID) {
			parse_declaration(DECL_CTX_FOR_INIT);
		}
		else if (token != T_SEMI) {
			parse_assignment_expr();
		}

		check_token(T_SEMI);
		next_token();

		emit(OP_LABEL);
		emit(startLabel);

		if (token != T_SEMI) {
			parse_assignment_expr();
		}

		emit(OP_JMPZ);
		emit(endLabel);

		check_token(T_SEMI);
		next_token();

		emit(OP_FOR);
		int incrementEndRef = emit(0);
		int bodyEndRef = emit(0);

		if (token != T_RPAREN) {
			parse_assignment_expr();
		}
		opcodes[incrementEndRef] = nopcodes;

		check_token(T_RPAREN);
		next_token();

		parse_stmt();

		opcodes[bodyEndRef] = nopcodes;

		emit(OP_JMP);
		emit(startLabel);

		emit(OP_LABEL);
		emit(endLabel);

		return;
	}

	// ;
	if (token == T_SEMI) {
		next_token();
		return;
	}

	//
	if (token == T_LCURLY) {
		next_token();

		parse_stmts(T_RCURLY);
		return;
	}

	parse_assignment_expr();
	//printf(";\n");
	check_token(T_SEMI);
	next_token();
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

void emit_push(int val_type, int val, int size) {
	emit(OP_PUSH);
	emit(val_type);
	emit(val);
	emit(size);
}

void start_func_decl(const char* s) {
	strcpy(funcname, s);

	add_extern(s);

	nparams = 0;
}

void start_func_body() {
	nlocals = 0;
	nlabels = 0;
	stack_size = 0;
	emit(OP_FUNC_PROLOGUE);
}

void end_func() {
	emit(OP_FUNC_EPILOGUE);

	gen_code(0, nopcodes);
	nopcodes = 0;
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

int add_global(const char* s) {
	strcpy(globals[nglobals++], s);
	return nglobals - 1;
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
	printf(".const\n");

	for (int i = 0; i < cpool_size; ) {
		int size = strlen(&cpool[i]) + 1;
		printf("__str_%d db ", i);

		bool quoted = false;
		for (int j = 0; j < size; ++j) {
			char ch = cpool[i + j];
			if (ch < ' ') {
				if (quoted) printf("', ", ch);
				else if (j != 0) printf(", ");
				printf("%02XH", ch);
				quoted = false;
			}
			else {
				if (j != 0 && !quoted) printf(", '");
				else if (!quoted) printf("'");
				printf("%c", ch);
				quoted = true;
			}
		}

		printf("\n");

		i += size;
	}
	printf("\n");
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
		printf("public %s\n", globals[i]);
	}

	printf(".data\n");
	for (int i = 0; i < nglobals; ++i) {
		int* type_info = &global_vars[i][1];
		int type_info_size = global_vars[i][0];
		int basic_type = get_basic_type(type_info, type_info_size);
		const char* basic_type_name = get_basic_type_asm_name(types_sizes[basic_type]);

		int array_size = get_type_array_size(type_info, type_info_size);

		
		if (type_info[type_info_size - 1] == DECL_ARRAY) {
			printf("%s %s %d dup (0)\n", globals[i], basic_type_name, array_size);
		}
		else {
			printf("%s %s 0\n", globals[i], basic_type_name);
		}
	}
	printf("\n");
}

void emit_asm_cmp(const char* op) {
	printf("  pop eax\n");
	printf("  pop ecx\n");
	printf("  cmp ecx, eax\n");
	printf("  %s al\n", op);
	printf("  cbw\n");
	printf("  cwde\n");
	printf("  push eax\n");
}

void emit_asm_binop(const char* op) {
	printf("  pop eax\n");
	printf("  pop ecx\n");
	printf("  %s ecx, eax\n", op);
	printf("  push ecx\n");
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

void gen_code(int from, int end) {
	for (int i = from; i < end; ) {
		int op = opcodes[i++];
		if (op == OP_FUNC_PROLOGUE) {
			printf("%s proc\n", funcname);
			printf("  push ebp\n");
			printf("  mov ebp, esp\n");
			printf("  sub esp, %d\n\n", stack_size);
		}
		else if (op == OP_FUNC_EPILOGUE) {
			printf("\n__exit_%s:\n", funcname);
			printf("  mov esp, ebp\n");
			printf("  pop ebp\n");
			printf("  ret 0\n");
			printf("%s endp\n\n", funcname);
		}
		else if (op == OP_PUSH) {
			int value_type = opcodes[i++];
			int value = opcodes[i++];
			int size = opcodes[i++];

			if (value_type == VAL_STR) {
				printf("  push OFFSET __str_%d\n", value);
			}
			else if (value_type == VAL_INT) {
				printf("  push %d\n", value);
			}
			else if (value_type == VAL_LOCAL) {
				if (value >= 0) {
					printf("  mov eax, DWORD PTR [ebp]+%d\n", value);
				}
				else {
					const char *instr = "XXX";
					switch (size) {
					case 1:
						instr = "movsx eax, BYTE PTR";
						break;

					case 2:
						instr = "movsx eax, WORD PTR";
						break;

					case 4:
						instr = "mov eax, DWORD PTR";
						break;
					}

					printf("  %s [ebp]%d\n", instr, value);
					
				}
				printf("  push eax\n", value);
			}
			else if (value_type == VAL_LOCAL_ADDR) {
				if (value >= 0) {
					printf("  lea eax, DWORD PTR [ebp]+%d \n", value);
				}
				else {
					printf("  lea eax, DWORD PTR [ebp]%d \n", value);
				}

				printf("  push eax\n", value);
			}
			else if (value_type == VAL_EXTERN) {
				printf("  push OFFSET %s\n", externs[value]);
			}
			else if (value_type == VAL_GLOB) {
				printf("  mov eax, DWORD PTR %s\n", globals[value]);
				printf("  push eax\n", value);
			}
			else if (value_type == VAL_GLOB_ADDR) {
				printf("  push OFFSET %s\n", globals[value]);
			}
		}
		else if (op == OP_DEREF) {
			int item_size = opcodes[i++];
			printf("  pop ecx\n");
			printf("  pop eax\n");
			printf("  imul ecx, %d\n", item_size);
			printf("  mov eax, DWORD PTR [eax + ecx]\n");
			printf("  push eax\n");
		}
		else if (op == OP_DEREF_ADDR) {
			int item_size = opcodes[i++];
			printf("  pop ecx\n");
			printf("  pop eax\n");
			printf("  imul ecx, %d\n", item_size);
			printf("  lea eax, DWORD PTR [eax + ecx]\n");
			printf("  push eax\n");
		}
		else if (op == OP_SAVE) {
			int size = opcodes[i++];

			printf("  pop eax\n");
			printf("  pop ecx\n");
			printf("  mov %s PTR [ecx], %s\n", get_asm_type(size), get_asm_reg(size));
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
			printf("  call DWORD PTR [esp+%d]\n", st);
			printf("  add esp, %d\n", st+4); // size of paremters + pointer to function
			printf("  push eax\n");
		}
		else if (op == OP_RETURN) {
			printf("  pop eax\n");
			printf("  jmp __exit_%s\n", funcname);
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
			printf("  jmp __%s_%d\n", funcname, label);
		}
		else if (op == OP_JMPZ) {
			printf("  pop eax\n");
			printf("  test eax, eax\n");

			int label = opcodes[i++];
			printf("  je __%s_%d\n", funcname, label);
		}
		else if (op == OP_JMPNZ) {
			printf("  pop eax\n");
			printf("  test eax, eax\n");

			int label = opcodes[i++];
			printf("  jne __%s_%d\n", funcname, label);
		}
		else if (op == OP_LABEL) {
			int id = opcodes[i++];
			printf("\n__%s_%d:\n", funcname, id);
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
			printf("  pop ecx\n");
			printf("  pop eax\n");
			printf("  cdq\n");
			printf("  idiv ecx\n", op);
			printf("  push eax\n");
		}
		else if (op == OP_MOD) {
			printf("  pop ecx\n");
			printf("  pop eax\n");
			printf("  cdq\n");
			printf("  idiv ecx\n", op);
			printf("  push edx\n");
		}
		else if (op == OP_INC || op == OP_DEC) {
			printf("  pop eax\n");
			printf("  %s DWORD PTR [eax], 1\n", op == OP_INC ? "add" : "sub");
			printf("  mov eax, DWORD PTR [eax]\n");
			printf("  push eax\n");
		}
		else if (op == OP_INC_POST || op == OP_DEC_POST) {
			printf("  pop ecx\n");
			printf("  mov eax, DWORD PTR [ecx]\n");
			printf("  %s DWORD PTR  [ecx], 1\n", op == OP_INC_POST ? "add" : "sub");
			printf("  push eax\n");
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
			printf("  pop ecx\n");
			printf("  xor eax, eax\n");
			printf("  cmp ecx, eax\n");
			printf("  sete al\n");
			printf("  push eax\n");
		}
	}
}

void gen_asm() {
	// header
	printf(".586\n");
	printf(".model flat, c\n\n");
	printf("includelib msvcrtd\n");
	printf("printf PROTO C :VARARG\n");
	printf("strcpy PROTO C\n");

	printf(".code\n\n");

	lineno = 1;
	next_token();
	parse_stmts(T_EOF);

	gen_globals();
	gen_cpool();

	printf("end\n");
	
}
// 

void dump_type(int* type_info, int size) {
	int array_size = get_type_array_size(type_info, size);
	int basic_type = get_basic_type(type_info, size);
	printf("!!! TYPE basic_type: %s, size %d\n", types[basic_type], array_size);

	for (int i = size - 1; i >= 0; --i) {
		switch (type_info[i]) {
		case DECL_ARRAY:
			printf("[%d] of ", type_info[--i]);
			break;

		case DECL_PTR:
			printf("ptr to ");
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

	case DECL_PTR:
		*dest_size = type_info_size - 1;
		break;

	case DECL_BASIC:
		*dest_size = type_info_size;
		break;
	}
	memcpy(dest_type_info, type_info, *dest_size * sizeof(type_info[0]));
}

void set_type_placeholder() {
	type_info_size = 0;
	type_info[type_info_size++] = TYPE_INT;
	type_info[type_info_size++] = DECL_BASIC;
}

int main(int argc, char** argv) {

	if (argc != 2) return -1;

	add_extern("fopen");
	add_extern("fclose");
	add_extern("fgetc");
	add_extern("ungetc");
	add_extern("printf");
	add_extern("exit");
	add_extern("strcpy");

	f = fopen(argv[1], "r");
	gen_asm();

	fclose(f);

}