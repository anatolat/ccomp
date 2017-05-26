#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "lexer.h"
#include "token.h"
#include "types.h"
#include "codegen.h"
#include "context.h"
#include "type_info.h"
#include "preprocessor.h"

FILE* ftarget;


enum {
	SCOPE_BLOCK,
	SCOPE_LOOP,
	SCOPE_SWITCH
};

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

int last_value_ref;

int add_param(const char* s);
int add_local(const char* s);

int get_local(const char* s);
int get_param(const char* s);

void start_func_decl(const char* s);
void start_func_body();
void end_func();
// end


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

	switch (token) {
	case T_INT_LIT: {
		emit_push(VAL_INT, token_num, 0);

		type_info_size = 0;
		type_info[type_info_size++] = TYPE_INT;
		type_info[type_info_size++] = DECL_BASIC;

		next_token();
		return;
	}
	case T_STR_LIT: {
		int cid = add_str(token_id);
		emit_push(VAL_STR, cid, 0);

		type_info_size = 0;
		type_info[type_info_size++] = TYPE_CHAR;
		type_info[type_info_size++] = DECL_BASIC;
		type_info[type_info_size++] = DECL_PTR;

		next_token();
		return;
	}
	case T_CHAR_LIT: {
		emit_push(VAL_INT, token_num, 0);

		type_info_size = 0;
		type_info[type_info_size++] = TYPE_INT;
		type_info[type_info_size++] = DECL_BASIC;

		next_token();
		return;
	}
	case T_LPAREN: {
		next_token();

		parse_assignment_expr();

		parse_token(T_RPAREN);
		return;
	}
	default:
		if (check_token(T_ID)) {
			int id = get_local(token_id);
			if (id != -1) {
				int offs = local_vars[id][0];

				type_info_size = copy_type_info(type_info, &local_vars[id][2], local_vars[id][1]);

				int is_addr = type_info[type_info_size - 1] == DECL_ARRAY;
				int size = get_type_byte_size(type_info, type_info_size);

				emit_push(is_addr ? VAL_LOCAL_ADDR : VAL_LOCAL, offs, size);
			}
			else if ((id = get_param(token_id)) != -1) {
				emit_push(VAL_LOCAL, 4 * (id + 2), 0);

				set_int_type();
			}
			else if ((id = get_global(token_id)) != -1) {
				type_info_size = copy_type_info(type_info, &global_vars[id][2], global_vars[id][1]);

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

			type_info_size = copy_type_info(type_info, item_type_info, item_type_info_size);
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
	switch (token) {
	case T_INC: case T_DEC: {
		int inc = token == T_INC;
		next_token();

		parse_unary_expr();

		convert_to_addr(last_value_ref);
		last_value_ref = nopcodes;

		emit(inc ? OP_INC : OP_DEC);

		set_int_type();
		return;
	}
	case T_NOT: {
		next_token();

		parse_unary_expr();

		last_value_ref = nopcodes;
		emit(OP_NOT);

		set_int_type();
		return;
	}
	case T_ADD: {
		next_token();

		parse_unary_expr();

		set_int_type();
		return;
	}
	case T_SUB: {
		next_token();

		parse_unary_expr();

		last_value_ref = nopcodes;
		emit(OP_NEG);

		set_int_type();
		return;
	}
	case T_BIT_AND: {
		next_token();

		parse_unary_expr();

		convert_to_addr(last_value_ref);

		set_int_type(); // FIXME
		return;
	}
	case T_MUL: {
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

		type_info_size = copy_type_info(type_info, item_type_info, item_type_info_size);
		return;
	}
	case T_SIZEOF: {
		next_token();

		int code = nopcodes;
		parse_unary_expr();

		nopcodes = code;

		int result = get_type_byte_size(type_info, type_info_size);
		emit_push(VAL_INT, result, 0);
		return;
	}

	default:
		parse_postfix_expr();
	}
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

int parse_decl_spec() {
	int attrs = 0;
	if (try_parse_token(T_EXTERN)) {
		attrs = ATTR_EXTERN;
	}

	// consts are not supported yet
	try_parse_token(T_CONST);

	check_token(T_TYPEID);
	
	type_info_size = 0;
	type_info[type_info_size++] = token_num;
	type_info[type_info_size++] = DECL_BASIC;

	next_token();

	return attrs;
}

// declaration_specifiers
int parse_decl_specs() {
	return parse_decl_spec();
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
void parse_declarator(int ctx, int attrs) {
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

			int id = get_global(funcname);
			global_vars[id][0] = ATTR_PUBLIC;

			start_func_body();

			parse_stmts(T_RCURLY);
			end_func();
		}
		else {
			parse_token(T_SEMI);

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
				add_global(id, attrs);
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
	int attrs = parse_decl_specs();
	parse_declarator(ctx, attrs);
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
	switch (token) {
	case T_TYPEID: case T_CONST: case T_EXTERN:
		parse_declaration(DECL_CTX_DEFAULT);
		return;

	case T_ENUM:
		parse_enum();
		return;

	case T_RETURN: {
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
	case T_IF: {
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
	case T_SWITCH: {
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
	case T_CASE: {
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
	case T_DEFAULT: {
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
	case T_WHILE: {
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
	case T_DO: {
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
	case T_FOR: {
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
	case T_BREAK: {
		next_token();

		emit(OP_JMP);
		emit(find_break_label());

		parse_token(T_SEMI);
		return;
	}
	case T_CONTINUE: {
		next_token();

		emit(OP_JMP);
		emit(find_continue_label());

		parse_token(T_SEMI);
		return;
	}
	case T_SEMI: {
		next_token();
		return;
	}
	case T_LCURLY: {
		start_block_scope();

		next_token();

		parse_stmts(T_RCURLY);

		end_scope();
		return;
	}

	default:
		parse_assignment_expr();
		parse_token(T_SEMI);
		break;
	}
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


void start_func_decl(const char* s) {
	strcpy(funcname, s);

	int id = add_global(s, ATTR_EXTERN);
	global_vars[id][1] = copy_type_info(&global_vars[id][2], type_info, type_info_size);

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

	gen_code(ftarget, funcname, 0, nopcodes);
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
	local_vars[index][1] = copy_type_info(&local_vars[index][2], type_info, type_info_size);

	return index;
}

int get_local(const char* s) {
	for (int i = nlocals - 1; i >= 0; --i) {
		if (!strcmp(locals[i], s)) return i;
	}
	return -1;
}

void compile() {
	// header
	fprintf(ftarget, ".586\n");
	fprintf(ftarget, ".model flat\n\n");
	fprintf(ftarget, "includelib msvcrtd\n");

	fprintf(ftarget, ".code\n\n");

	lineno = 1;
	next_token();
	parse_stmts(T_EOF);

	gen_globals(ftarget);
	gen_cpool(ftarget);

	fprintf(ftarget, "end\n");
}
// 


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

	add_global(s, ATTR_EXTERN);
}

const char* change_ext(const char* s, const char* ext) {
	int dot = -1;
	for (int i = 0; s[i]; ++i) {
		if (s[i] == '\\' || s[i] == '/') {
			dot = -1;
		}
		else if (s[i] == '.') {
			dot = i;
		}
	}

	int ext_len = strlen(ext);
	int src_len = dot == -1 ? strlen(s) : dot;
	
	const char* dst = malloc(src_len + ext_len + 1);
	memcpy(dst, s, src_len);
	memcpy(dst + src_len, ext, ext_len + 1);

	return dst;
}

int main(int argc, char** argv) {

	if (argc != 3)  {
		printf("Usage ccomp <source.c> <target.asm>\n");
		return -1;
	}

	add_int_const("EOF", -1);

	add_extern_func("fopen");
	add_extern_func("fclose");
	add_extern_func("fgetc");
	add_extern_func("fputc");
	add_extern_func("ungetc");
	add_extern_func("fprintf");
	add_extern_func("printf");
	add_extern_func("exit");
	add_extern_func("strcpy");
	add_extern_func("strcmp");
	add_extern_func("strlen");
	add_extern_func("memcpy");
	add_extern_func("malloc");
	add_extern_func("free");

	add_type("void", 0);
	add_type("char", 1);
	add_type("int", 4);
	add_type("FILE", 4);

	const char* sfile = change_ext(argv[2], ".s");

	preprocess(sfile, argv[1]);

	fsource = fopen(sfile, "r");
	ftarget = fopen(argv[2], "w");

	compile();

	fclose(fsource);
	fclose(ftarget);

	free(sfile);
}