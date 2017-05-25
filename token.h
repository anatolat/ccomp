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

const char* tok2str(int tok);