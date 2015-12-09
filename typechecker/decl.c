#include "decl.h"
#include "expr.h"
#include "symbol.h"
#include "scope.h"
#include "type.h"
#include "param_list.h"

extern int error_count;

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
		printf(" =\n{\n");
		stmt_print(d->code,indent+4);		// prints the statements inside
		printf("}\n");
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

// resolves the decl. 
// @input: pointer to a struct decl d
// @output: a count of the resolution errors, and a linking of all names to their respective symbols
void decl_resolve( struct decl *d )
{
	if(!d) return;		// base case, stop when you are not resolving a decl anymore
	struct symbol *sym;	// pointer to a symbol that will be created when a variable is declared
	if(scope_lookup_single(d->name)){		// if the name is delcared in the current scope
		if(scope_lookup_single(d->name)->type->kind!=TYPE_FUNCTION || scope_lookup_single(d->name)->code==1){	// if the decl is a function and there previous declaration already
			printf("resolve error: %s is already declared in this scope\n",d->name);	// print error
			error_count++;									// increment global count
		} else {										// if the name was already in the scope
			if(d->code){									// if there is code in the decl (function decl)
				if(!type_compare(scope_lookup_single(d->name)->type->subtype,d->type->subtype) || !param_list_compare(d->type->params,scope_lookup_single(d->name)->type->params)){
					printf("resolve error: %s is already declared in this scope with different parameters/return values\n",d->name);
					error_count++;
				} else {
					d->symbol = scope_lookup_single(d->name);				// set the symbol to the same symbol as the function prototype
					printf("%s resolves to global %s\n",d->name,d->symbol->name);		// print info message
					d->symbol->code = 1;
					// enter a new scope and resolve the param and code inside of the new scope
					scope_enter();
					param_list_resolve(d->type->params);
					stmt_resolve(d->code);
					// leave the scope after resolution
					scope_exit();
				}
			} else {									// if this is a regular declaration that is already found with scope_lookup()
				printf("resolve error: %s is already declared in this scope\n",d->name);
				error_count++;
			}
		}
	} else {
		if(scope_level() == 1) {					// case of a global scope
			sym = symbol_create(SYMBOL_GLOBAL,d->type,d->name);	// make a global symbol	
			printf("%s resolves to global %s\n",d->name,sym->name);	// print message
		// case of non-global scope: create a local symbol, and increment the local variable count in the HT. print a message
		} else {
			sym = symbol_create(SYMBOL_LOCAL,d->type,d->name);
			int *local_count = hash_table_lookup(h,"0local_count");
			sym->which = *local_count;
			(*local_count)++;
			printf("%s resolves to local %d\n",d->name,sym->which);
		}
		// assign the new symbol to the symbol of the current decl
		d->symbol = sym;
		// insert into HT
		scope_bind(d->name,sym);
		expr_resolve(d->value);		// resolve the expr that the decl is assigned to
		// case of a function prototype
		if(!(d->code) && d->type->kind==TYPE_FUNCTION){	
			// enter a scope and resolve the param list
			scope_enter();
			param_list_resolve(d->type->params);
			scope_exit();
		}
		// case of function delcaration with code: indicate that this symbol has code, enter a scope, and resolve params and code
		if(d->code){
			sym->code = 1;
			scope_enter();
			param_list_resolve(d->type->params);
			stmt_resolve(d->code);
			scope_exit();
		}
	}
	// resolve the next decl in the linked lists of decls
	decl_resolve(d->next);
}

// function that verifies that there are no type errors.
// @input: decl pointer d
// @output: error_count is incremented with any error, and decls are recursively typechecked
void decl_typecheck( struct decl *d )
{	
	if(!d) return;		// base case
	// if the decl has a value
	if(d->value){
		// case where the decl is global and the thing it is assigned to is not constant: error because decls of global must be constant
		if(d->symbol->kind == SYMBOL_GLOBAL && !expr_is_constant(d->value)){	
		printf("type error: declaration of global variable %s is not constant (",d->name);
		expr_print(d->value);
		printf(")\n");
		error_count++;
		}
	}
	// case where the decl is not a function and the type of the variable and its expression do not match: increment error_count and warn user
	if(d->value && d->type->kind != TYPE_FUNCTION && !type_compare(d->type,expr_typecheck(d->value))){
		printf("type error: cannot assign type ");
		type_print(d->type);
		printf(" (%s) to type ",d->name);
		type_print(expr_typecheck(d->value));
		printf(" (");
		expr_print(d->value);
		printf(")\n");
		error_count++;
	}
	// case where the decl has code (case of a function declaration)
	if(d->code){
		stmt_typecheck(d->code,d);		// recursively typecheck the linked list of statements
	}
	// typecheck the next decl in the linked list
	decl_typecheck(d->next);
}
