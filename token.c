#include "token.h"

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

