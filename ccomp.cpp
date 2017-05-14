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
	, T_LESS
	, T_GREATER
	, T_EQ
	, T_OR
	, T_AND
	, T_BIT_OR
	, T_BIT_AND

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

int ntypes = 3 ;
char* types[256] = {
	"void",
	"char",
	"int",
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
	OP_LOAD
	, OP_ADDR
	, OP_PUSH
	, OP_SAVE
	, OP_CALL
	, OP_RETURN
	, OP_JMP
	, OP_JMPZ
	, OP_LABEL
	, OP_ADD
	, OP_SUB
	, OP_MUL
	, OP_LESS
	, OP_EQ
	, OP_FOR
	, OP_INC
	, OP_DEC
	, OP_INC_POST
	, OP_DEC_POST
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

int nopcodes = 0;
int opcodes[1024];

int emit(int);

char funcname[256];

int nparams;
char params[256][64];

int nlocals;
char locals[256][64];

int nlabels;

int nexterns;
char externs[256][64];

int nglobals;
char globals[256][64];

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
void gen_func_prologue(const char* name);
void gen_func_epilogue(const char* name);
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
	case T_ADD: return "ADD";
	case T_SUB: return "SUB";
	case T_MUL: return "MUL";
	case T_LESS: return "LESS";
	case T_GREATER: return "GREATER";
	case T_OR: return "OR";
	case T_AND: return "AND";
	case T_BIT_OR: return "BIT_OR";
	case T_BIT_AND: return "BIT_AND";
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

		// ignore preprocessor directives
		if (ch == '#') {
			while (ch != '\n' && ch != EOF) {
				ch = fgetc(f);
			}
		}
		// comments
		else if (ch == '/') {
			ch = fgetc(f);
			if (ch == '/') {
				while (ch != '\n' && ch != EOF) {
					ch = fgetc(f);
				}
			}
			else if (ch == '*') {
				while (ch != EOF) {
					ch = fgetc(f);
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
	if (ch == '<') return T_LESS;
	if (ch == '>') return T_GREATER;

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

	if (ch == '!') return T_NOT;

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


	printf("Invalid token %c\n", ch);
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
	if (token == T_CHAR_LIT) {
		emit(OP_PUSH);
		emit(VAL_INT);
		emit(token_num);

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
		else if ((id = get_global(token_id)) != -1) {
			emit(OP_PUSH);
			emit(VAL_GLOB);
			emit(id);
		}
		else {
			printf("Unknown identifier %s\n", token_id);
		}
		next_token();
		return;
	}
	
	next_token();
}

void convert_to_addr(int code) {
	if (opcodes[code] == OP_PUSH && opcodes[code + 1] == VAL_LOCAL) {
		opcodes[code + 1] = VAL_LOCAL_ADDR;
	}
	else if (opcodes[code] == OP_PUSH && opcodes[code + 1] == VAL_GLOB) {
		opcodes[code + 1] = VAL_GLOB_ADDR;
	}
}

void parse_postfix_expr() {
	int code = nopcodes;
	parse_primary_expr();

	if (token == T_LPAREN) {
		next_token();

		emit(OP_CALL);
		int sizes[256] = {}; // size of the code generated for each parameter
		int callHeader = emit(0);

		int paramCount = 0;
		while (token != T_RPAREN && token != T_EOF) {
			sizes[paramCount++] = nopcodes;

			parse_assignment_expr();

			if (token == T_COMMA) {
				next_token();
				continue;
			}
		}

		check_token(T_RPAREN);
		next_token();

		opcodes[callHeader] = emit(paramCount);
		for (int i = 0; i < paramCount; ++i) {
			emit(sizes[i]);
		}
		return;
	}

	if (token == T_INC || token == T_DEC) {
		bool inc = token == T_INC;
		next_token();

		convert_to_addr(code);
		emit(inc ? OP_INC_POST : OP_DEC_POST);
	}
}

void parse_unary_expr() {
	if (token == T_INC || token == T_DEC) {
		bool inc = token == T_INC;
		next_token();
		
		int code = nopcodes;
		parse_unary_expr();

		convert_to_addr(code);

		emit(inc ? OP_INC : OP_DEC);
		return;
	}
	
	parse_postfix_expr();
}

void parse_assignment_expr() {
	int lhs = nopcodes;
	parse_unary_expr();

	if (token == T_ASSIGNMENT) {
		next_token(); 
		parse_assignment_expr();

		convert_to_addr(lhs);
		emit(OP_SAVE);
	}

	if (token == T_ADD || token == T_SUB || token == T_MUL || token == T_LESS || token == T_EQ) {
		int op = token;
		next_token();
		parse_postfix_expr();

		if (op == T_ADD) emit(OP_ADD);
		else if (op == T_SUB) emit(OP_SUB);
		else if (op == T_MUL) emit(OP_MUL);
		else if (op == T_LESS) emit(OP_LESS);
		else if (op == T_EQ) emit(OP_EQ);
	}
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
	
	while (true) {
		if (token == T_LBRACKET) {
			while (token != T_RBRACKET && token != T_EOF) {
				next_token();
			}
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
}

// declarator : pointer? direct_declarator
void parse_declarator(int ctx) {
	// pointer
	while (token == T_MUL) {
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
				add_local(id);
			}
			else {
				add_global(id);
			}
		}

		if (token == T_ASSIGNMENT && ctx != DECL_CTX_PARAM) {
			next_token();

			emit(OP_PUSH);
			emit(VAL_LOCAL_ADDR);

			int index = get_local(id);
			emit(-4 * (index + 1));

			parse_assignment_expr();

			emit(OP_SAVE);
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

		emit(OP_JMPZ);
		int endLabel = nlabels++;
		emit(endLabel);

		next_token();
		parse_stmt();

		emit(OP_LABEL);
		emit(endLabel);

		if (token == T_ELSE) {
			next_token();

			parse_stmt();
		} 

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

void start_func_decl(const char* s) {
	strcpy(funcname, s);

	add_extern(s);

	nparams = 0;
}

void start_func_body() {
	gen_func_prologue(funcname);
	nlocals = 0;
	nlabels = 0;
}

void end_func() {
	gen_code(0, nopcodes);
	nopcodes = 0;

	gen_func_epilogue(funcname);
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
void gen_func_prologue(const char* name) {
	printf("%s proc\n", name);
	printf("  push ebp\n");
	printf("  mov ebp, esp\n");
	printf("  sub esp, 72\n\n"); // FIXME

}

void gen_func_epilogue(const char* name) {
	printf("\n");
	printf("__exit_%s:\n", funcname);
	printf("  mov esp, ebp\n");
	printf("  pop ebp\n");
	printf("  ret 0\n");

	printf("%s endp\n\n", name);
}

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

void gen_globals() {
	// public section
	for (int i = 0; i < nglobals; ++i) {
		printf("public %s\n", globals[i]);
	}

	printf(".data\n");
	for (int i = 0; i < nglobals; ++i) {
		printf("%s dd 0\n", globals[i]);
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

void gen_code(int from, int end) {
	for (int i = from; i < end; ) {
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

				printf("  push eax\n", value);
			}
			else if (valueType == VAL_EXTERN) {
				printf("  push OFFSET %s\n", externs[value]);
			}
			else if (valueType == VAL_GLOB) {
				printf("  mov eax, DWORD PTR %s\n", globals[value]);
				printf("  push eax\n", value);
			}
			else if (valueType == VAL_GLOB_ADDR) {
				printf("  lea eax, DWORD PTR %s\n", globals[value]);
				printf("  push eax\n", value);
			}
		}
		else if (op == OP_SAVE) {
			printf("  pop eax\n");
			printf("  pop ecx\n");
			printf("  mov  DWORD PTR [ecx], eax\n");
		}
		else if (op == OP_CALL) {
			int headerRef = opcodes[i];
			int paramCount = opcodes[headerRef];

			i = headerRef + paramCount + 1;

			int paramEnd = headerRef;
			for (int param = paramCount - 1; param >= 0; --param) {
				int paramOffset = opcodes[headerRef + param + 1];
				gen_code(paramOffset, paramEnd);

				paramEnd = paramOffset;
			}

			// FIXME: support different parameter sizes
			int st = paramCount * 4;
			printf("  call DWORD PTR [esp+%d]\n", st);
			printf("  add esp, %d\n", st+4); // size of paremters + pointer to function
			printf("  push eax\n");
		}
		else if (op == OP_RETURN) {
			printf("  pop eax\n");
			printf("  jmp SHORT __exit_%s\n", funcname);
		}
		else if (op == OP_FOR) {
			// post increment instructions after body

			int incrementEnd = opcodes[i++];
			int bodyEnd = opcodes[i++];
			gen_code(incrementEnd, bodyEnd);
			gen_code(i, incrementEnd);
			
			i = bodyEnd;
		}
		else if (op == OP_JMP) {
			int label = opcodes[i++];
			printf("  jmp SHORT __%s_%d\n", funcname, label);
		}
		else if (op == OP_JMPZ) {
			printf("  pop eax\n");
			printf("  test eax, eax\n");

			int label = opcodes[i++];
			printf("  je SHORT __%s_%d\n", funcname, label);
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
		else if (op == OP_INC || op == OP_DEC) {
			printf("  pop eax\n");
			printf("  %s DWORD PTR [eax], 1\n", op == OP_INC ? "add" : "sub");
			printf("  push ecx\n");
		}
		else if (op == OP_INC_POST || op == OP_DEC_POST) {
			printf("  pop ecx\n");
			printf("  mov eax, DWORD PTR [ecx]\n");
			printf("  %s DWORD PTR  [ecx], 1", op == OP_INC_POST ? "add" : "sub");
			printf("  push eax\n");
		}
		else if (op == OP_LESS) {
			emit_asm_cmp("setl");
		}
		else if (op == OP_EQ) {
			emit_asm_cmp("sete");
		}
	}
}

void gen_asm() {
	// header
	printf(".586\n");
	printf(".model flat, c\n\n");

	printf(".code\n\n");

	next_token();
	parse_stmts(T_EOF);

	gen_globals();
	gen_cpool();

	printf("end\n");
	
}
// 

int main(int argc, char** argv) {

	if (argc != 2) return -1;


	add_extern("fopen");
	add_extern("fclose");
	add_extern("fgetc");
	add_extern("ungetc");
	add_extern("printf");
	add_extern("exit");

	f = fopen(argv[1], "r");
	gen_asm();

	fclose(f);

}