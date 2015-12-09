#include "param_list.h"
#include <string.h>

extern int error_count;

struct param_list * param_list_create(char *name, struct type *type, struct param_list *next )
{
	struct param_list *p = malloc(sizeof(*p));
	p->name = name;
	p->type = type;
	p->symbol = NULL;
	p->next = next;

	return p;
}

void param_list_print(struct param_list *p)
{
	if(!p) return;
	
	printf("%s: ",p->name);
	type_print(p->type);
	if(p->next!=0){
		printf(", ");
	}
	param_list_print(p->next);
}

// resolve the param_list by binding the paramaters to the scope of the function
// @inputs: a param_list to resolve
// @outputs: the number of errors produced by typechecking the param list
void param_list_resolve ( struct param_list *p )
{
	if(!p) return;		// base case

	// if the param is already in this scope
	if(scope_lookup_single(p->name)){		
		printf("resolve error: %s is already declared inthis scope\n",p->name);
		error_count++;
	// if it is not: create a new symbol, bind it to the HT, and print a message to the user
	} else {
		struct symbol *sym = symbol_create(SYMBOL_PARAM,p->type,p->name);	
		int *param_count = hash_table_lookup(h,"0param_count");
		if(*param_count >= 6) {									// check to see if function has more than 6 params
			fprintf(stderr,"error: function cannot have more than six parameters\n");
			exit(1);
		}
		sym->which = *param_count;
		(*param_count)++;
		scope_bind(p->name,sym);
		p->symbol = sym;
		printf("%s resolves to param %d\n",p->name,sym->which);
	}
	// resolve the next param_list object in the list
	param_list_resolve(p->next);
}

// FUNCTION NOT USED
struct param_list* param_list_copy(struct param_list *p)
{
	if(!p) return NULL;
		
	struct param_list* copy = param_list_create(p->name,p->type,p->next);
	copy->name = strdup(p->name);
	copy->type = type_copy(p->type);
	copy->symbol = scope_lookup_multiple(p->name);
	copy->next = param_list_copy(p->next);
	return copy;
}

// compares two param list objects
// @input: two param_list pointers to compare
// @output: 1 if the param lists are the same, 0 if they are not
int param_list_compare(struct param_list *a, struct param_list *b)
{
	if(!a && !b) return 1;		// they are the same if both are null
	if(!a || !b) return 0;		// they are different if one is null
	// if both params have names (they always should): compare all the members of a param_list struct
	if(a->name && b->name){
		return(strcmp(a->name,b->name)==0 && type_compare(a->type,b->type) && param_list_compare(a->next,b->next));
	// case where neither has a name, compare everything but the names to prevent segfault with strcmp
	} else if (!a->name && !b->name){
		return(type_compare(a->type,b->type) && param_list_compare(a->next,b->next));
	// case where one has a name and the other doesn't: they are not the same, so return 0
	} else {
		return 0;
	}
}

