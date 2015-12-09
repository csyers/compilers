#include "expr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct expr * expr_create(expr_t kind, struct expr *left, struct expr *right )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = kind;
	e->left = left;
	e->right = right;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = 0;
	e->string_literal = NULL;

	return e;
}

struct expr * expr_create_name( const char *n)
{
	struct expr *e = malloc(sizeof(*e));
	
	e->kind = EXPR_NAME;
	e->left = NULL;
	e->right = NULL;
	e->name = n;
	e->symbol = NULL;
	e->literal_value = 0;
	e->string_literal = NULL;
	
	return e;
}

struct expr * expr_create_boolean_literal( int c )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = EXPR_BOOL;
	e->left = NULL;
	e->right = NULL;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = c;
	e->string_literal = NULL;

	return e;
}

struct expr * expr_create_integer_literal( int c )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = EXPR_INT;
	e->left = NULL;
	e->right = NULL;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = c;
	e->string_literal = NULL;

	return e;
}

struct expr * expr_create_character_literal( int c )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = EXPR_CHAR;
	e->left = NULL;
	e->right = NULL;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = c;
	e->string_literal = NULL;

	return e;
}

struct expr * expr_create_string_literal( const char *str )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = EXPR_STR;
	e->left = NULL;
	e->right = NULL;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = 0;
	e->string_literal = str;

	return e;
}

void expr_print(struct expr *e)
{
	int i;
	if(!e) return;
	if(e->kind==EXPR_GROUP){
		printf("(");
	} else if (e->kind==EXPR_BLOCK){
		printf("{");
	}
	expr_print(e->left);
	switch(e->kind){
		case EXPR_ADD:
			printf("+");	
			break;
		case EXPR_SUB:
			printf("-");	
			break;
		case EXPR_MUL:
			printf("*");	
			break;
		case EXPR_DIV:
			printf("/");	
			break;
		case EXPR_NAME:
			printf("%s",e->name);	
			break;
		case EXPR_BOOL:
			if(e->literal_value){
				printf("true");
			} else {
				printf("false");
			}	
			break;
		case EXPR_INT:
			printf("%d",e->literal_value);	
			break;
		case EXPR_CHAR:
			if(e->literal_value==0){
				printf("\'\\0\'");
			} else if(e->literal_value==0x0A){
				printf("\'\\n\'");	
			} else {
				printf("\'%c\'",e->literal_value);	
			}
			break;
		case EXPR_STR:
			printf("\"");
			for(i = 0; i < strlen(e->string_literal); i++){
				if(e->string_literal[i]==0){
					printf("\\0");
				} else if(e->string_literal[i]==0x0A){
					printf("\\n");
				} else {
					printf("%c",e->string_literal[i]);
				}
			}
			printf("\"");
			break;
		case EXPR_OR:
			printf("||");	
			break;
		case EXPR_AND:
			printf("&&");	
			break;
		case EXPR_LT:
			printf("<");	
			break;
		case EXPR_GT:
			printf(">");	
			break;
		case EXPR_LE:
			printf("<=");	
			break;
		case EXPR_GE:
			printf(">=");	
			break;
		case EXPR_EQ:
			printf("==");	
			break;
		case EXPR_NE:
			printf("!=");	
			break;
		case EXPR_MOD:
			printf("%c",'%');	
			break;
		case EXPR_NEG:
			printf("-");	
			break;
		case EXPR_NOT:
			printf("!");	
			break;
		case EXPR_EXP:
			printf("^");	
			break;
		case EXPR_INCR:
			printf("++");	
			break;
		case EXPR_DECR:
			printf("--");	
			break;
		case EXPR_LIST:
			printf(",");	
			break;
		case EXPR_FUNC:
			printf("(");
			break;
		case EXPR_ARRAY:
			printf("[");
			break;
		case EXPR_ASSIGN:
			printf("=");
			break;
		case EXPR_BLOCK:
			printf("}");
	}
	expr_print(e->right);
	if(e->kind==EXPR_FUNC){
		printf(")");
	} else if (e->kind==EXPR_ARRAY){
		printf("]");
	}
	if(e->kind==EXPR_GROUP){
		printf(")");
	}
}
