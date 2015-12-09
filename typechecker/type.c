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
		default:
			//printf("this sucks");
			break;
	}
}

struct type* type_copy(struct type *t)
{
	if(!t) return NULL;
	struct type *copy = malloc(sizeof(*t));

	copy->kind = t->kind;
	copy->params = param_list_copy(t->params);
	copy->subtype = type_copy(t->subtype);
	copy->expr = expr_copy(t->expr);

	return copy;
}

int type_compare(struct type *a, struct type *b)
{
	if(!a && !b) return 1;
	if(!a || !b) return 0;
	//return (a->kind==b->kind && type_compare(a->subtype,b->subtype));
	return (a->kind==b->kind && param_list_compare(a->params,b->params) && type_compare(a->subtype,b->subtype) && expr_compare(a->expr,b->expr));
}

void type_delete(struct type *t)
{
	free(t->params);
	type_delete(t->subtype);
	free(t->expr);
	free(t);
}
