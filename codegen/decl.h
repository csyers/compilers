
#ifndef DECL_H
#define DECL_H

#include "type.h"
#include "stmt.h"
#include "expr.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>

extern int reg[16];

struct decl {
	char *name;
	struct type *type;
	struct expr *value;
	struct stmt *code;
	struct symbol *symbol;
	struct decl *next;
};

struct decl * decl_create( char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next );
void decl_print( struct decl *d, int indent );

void decl_resolve( struct decl *d);		// resolves the decl, counts the number of resolve errors

void decl_typecheck ( struct decl *d);		// typechecks the decl, and count s the number of typecheck error

void decl_codegen(struct decl *d, FILE* f);   // generates assembly for a decl

#endif


