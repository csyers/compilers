#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>
#include "global.h"
#include "register.h"

int error_count;

typedef enum {
	EXPR_ADD,
	EXPR_SUB,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_NAME,
	EXPR_BOOL,
	EXPR_INT,
	EXPR_CHAR,
	EXPR_STR,
	EXPR_OR,
	EXPR_AND,
	EXPR_LT,
	EXPR_GT,
	EXPR_LE,
	EXPR_GE,
	EXPR_EQ,
	EXPR_NE,
	EXPR_MOD,
	EXPR_NEG,
	EXPR_NOT,
	EXPR_EXP,
	EXPR_INCR,
	EXPR_DECR,
	EXPR_LIST,
	EXPR_FUNC,
	EXPR_ARRAY,
	EXPR_ASSIGN,
	EXPR_BLOCK,
	EXPR_GROUP
	/* many more types to add here */
} expr_t;

struct expr {
	/* used by all expr types */
	expr_t kind;
	struct expr *left;
	struct expr *right;

	/* used by leaf expr types */
	const char *name;
	struct symbol *symbol;
	int literal_value;
	const char * string_literal;
	int reg;
};

struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right );

struct expr * expr_create_name( const char *n );
struct expr * expr_create_boolean_literal( int c );
struct expr * expr_create_integer_literal( int c );
struct expr * expr_create_character_literal( int c );
struct expr * expr_create_string_literal( const char *str );

void expr_print( struct expr *e );

void expr_resolve( struct expr *e );

struct type * expr_typecheck( struct expr *e);

struct expr * expr_copy( struct expr *e);

int expr_compare( struct expr *a, struct expr *b);

int expr_is_constant( struct expr *e );

void expr_codegen( struct expr *e, FILE* f);

void expr_get_string (struct expr * e, FILE* f);

void expr_print_codegen (struct expr * e, FILE* f);

#endif
