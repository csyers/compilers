#include "symbol.h"
#include "register.h"


struct symbol * symbol_create( symbol_t kind, struct type *type, char *name)
{
	struct symbol *s = malloc(sizeof(*s));
	s->kind = kind;
	s->type = type;
	s->name = name;
	s->code = 0;
	s->local_count = -1;
	return s;
}

void symbol_code(struct symbol *s, char *buf)
{
	if(s->kind == SYMBOL_GLOBAL){
		sprintf(buf,"%s",s->name);
	}
	if(s->kind == SYMBOL_PARAM){
		sprintf(buf,"-%d(%%rbp)",8+8*s->which);
	}
	if(s->kind == SYMBOL_LOCAL){
		sprintf(buf,"-%d(%%rbp)",56+8*s->which);
	}
	return;
}
