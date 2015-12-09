#include "decl.h"



struct decl * decl_create(char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next)
{
	struct decl *d = malloc(sizeof(*d));
	d->name = name;
	d->type = t;
	d->value = v;
	d->code = c;
	d->symbol = NULL;
	d->next = next;

	return d;
}

// recursively prints the name, its code (if it has any), and the values for declarations
void decl_print(struct decl *d, int indent)
{
	if(!d) return;
	printf("%*s%s: ",indent,"",d->name);
	type_print(d->type);
	if(d->code){			// case where it is a function decl
		printf(" =\n");
		stmt_print(d->code,indent);		// prints the statements inside
	} else if(d->value && d->value->kind!=EXPR_LIST && d->type->kind!=TYPE_ARRAY){	
		printf(" = ");
		expr_print(d->value);
		printf(";");
	} else if(d->value && (d->value->kind==EXPR_LIST || d->type->kind==TYPE_ARRAY)){
		printf(" = {");
		expr_print(d->value);
		printf("};");
	} else {
		printf(";");
	}
	if(!d->code){
		printf("\n");
	}
	decl_print(d->next,indent);
}
