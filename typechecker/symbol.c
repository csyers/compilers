#include "symbol.h"

struct symbol * symbol_create( symbol_t kind, struct type *type, char *name)
{
	struct symbol *s = malloc(sizeof(*s));
	s->kind = kind;
	s->type = type;
	s->name = name;
	s->code = 0;
	return s;
}

int symbol_compare(struct symbol *a, struct symbol *b)
{
	if(!a) {
	printf("no symbol for a\n");
	}
	if(!b) {
	
	printf("no symbol for b\n");
	}
	if(!a && !b) return 1;
	if(!a || !b) return 0;
	printf("a->name: %s\n", a->name);
	printf("b->name: %d\n", b->name);
	printf("a->kind: %d\n", a->kind);
	printf("b->kind: %d\n", b->kind);
	printf("a->code: %d\n", a->code);
	printf("b->code: %d\n", b->code);
	printf("b->which: %d\n", a->which);
	printf("b->which: %d\n", b->which);
	printf("type_compare:a %d\n", type_compare(a->type,b->type));
	return(a->kind==b->kind && a->which==b->which && type_compare(a->type,b->type) && strcmp(a->name,b->name)==0 && a->code==b->code);
}
