
#ifndef STMT_H
#define STMT_H

#include "symbol.h"
#include "decl.h"
#include "scope.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	STMT_DECL,
	STMT_EXPR,
	STMT_IF_ELSE,
	STMT_FOR,
	STMT_PRINT,
	STMT_RETURN,
	STMT_BLOCK,
	STMT_EMPTY
} stmt_kind_t;

struct stmt {
	stmt_kind_t kind;
	struct decl *decl;
	struct expr *init_expr;
	struct expr *expr;
	struct expr *next_expr;
	struct stmt *body;
	struct stmt *else_body;
	struct stmt *next;
};

struct stmt * stmt_create( stmt_kind_t kind, struct decl *d, struct expr *init_expr, struct expr *e, struct expr *next_expr, struct stmt *body, struct stmt *else_body );
void stmt_print( struct stmt *s, int indent );

void stmt_resolve( struct stmt *s );

void stmt_typecheck( struct stmt *s, struct decl *d );
#endif
