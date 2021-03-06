#ifndef TOKEN_H
#define TOKEN_H


enum token{
	TOKEN_ERROR,
	TOKEN_IDENT,
	TOKEN_STRING_LITERAL,
	TOKEN_INTEGER_LITERAL,
	TOKEN_CHARACTER_LITERAL,
	TOKEN_ARRAY,
	TOKEN_BOOLEAN,
	TOKEN_CHAR,
	TOKEN_ELSE,
	TOKEN_FALSE,
	TOKEN_FOR,
	TOKEN_FUNCTION,
	TOKEN_IF,
	TOKEN_INTEGER,
	TOKEN_PRINT,
	TOKEN_RETURN,
	TOKEN_STRING,
	TOKEN_TRUE,
	TOKEN_VOID,
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_MOD,
	TOKEN_EXP,
	TOKEN_INCREMENT,
	TOKEN_DECREMENT,
	TOKEN_GT,
	TOKEN_GE,
	TOKEN_LT,
	TOKEN_LE,
	TOKEN_EQ,
	TOKEN_NE,
	TOKEN_ASSIGN,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_L_PAREN,
	TOKEN_R_PAREN,
	TOKEN_L_BRACE,
	TOKEN_R_BRACE,
	TOKEN_L_BRACKET,
	TOKEN_R_BRACKET,
	TOKEN_COMMA,
	TOKEN_LOGICAL_NOT
};
typedef enum token token_t;
const char* token_string(token_t t);
void edit_string(char* word);

#endif
