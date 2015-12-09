
#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"
#include <stdio.h>
#include <stdlib.h>


typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL
} symbol_t;

struct symbol {
	symbol_t kind;
	int which;
	struct type *type;
	char *name;
	int code;
	int local_count;	// used to keep track of the number of local variables in a function
};

struct symbol * symbol_create( symbol_t kind, struct type *type, char *name );

int symbol_compare ( struct symbol *a, struct symbol *b);

void symbol_code(struct symbol *s, char * buf);

#endif
