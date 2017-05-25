#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "token.h"
#include "types.h"

FILE* fsource;
int token;
char token_id[256];
int token_num;
int lineno;

char skipws() {
	char ch;
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

	return ch;
}

int next_token_helper() {
	int ch = skipws();

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

