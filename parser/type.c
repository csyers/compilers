#include "type.h"

#include <stdio.h>
#include <stdlib.h>


struct type * type_create(type_kind_t kind, struct param_list *params, struct type *subtype)
{
	struct type *t = malloc(sizeof(*t));
	
	t->kind = kind;
	t->params = params;
	t->subtype = subtype;
	t->expr = NULL;
	return t;
}

struct type * type_create_array(type_kind_t kind, struct param_list *params, struct type *subtype, struct expr *expr)
{
	struct type *t = malloc(sizeof(*t));
	
	t->kind = kind;
	t->params = params;
	t->subtype = subtype;
	t->expr = expr;
	return t;
}

void type_print( struct type *t)
{
	if(!t) return;
	
	switch(t->kind){
		case TYPE_BOOLEAN:
			printf("boolean");
			break;
		case TYPE_CHARACTER:
			printf("character");
			break;
		case TYPE_INTEGER:
			printf("integer");
			break;
		case TYPE_STRING:
			printf("string");
			break;
		case TYPE_ARRAY:
			printf("array [");
			expr_print(t->expr);
			printf("] ");
			type_print(t->subtype);
			break;
		case TYPE_FUNCTION:
			printf("function ");
			type_print(t->subtype);
			printf(" (");
			param_list_print(t->params);
			printf(")");
			break;
		case TYPE_VOID:
			printf("void");
			break;
	}
}
