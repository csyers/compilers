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
			//int *local_count = hash_table_lookup(h,"0local_count");
			//(*local_count)++;
			sym->which = scope_increment_local_count();
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
			if(hash_table_lookup(h,"0local_count")){
				int *local_count = hash_table_lookup(h,"0local_count");
				if(local_count){
					sym->local_count = *local_count;
				}
			}
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

void decl_codegen(struct decl *d, FILE* f)
{
	char reg[200];
	if(!d) return;
	
	if(d->symbol->kind == SYMBOL_GLOBAL){
		switch(d->type->kind){
			case TYPE_BOOLEAN:
			case TYPE_CHARACTER:
			case TYPE_INTEGER:
				if(d->value){
					fprintf(f,".data\n");
					symbol_code(d->symbol,reg);
					fprintf(f,"%s: .quad %d\n",reg,d->value->literal_value);
				} else {
					fprintf(f,".data\n");
					symbol_code(d->symbol,reg);
					fprintf(f,"%s: .quad 0\n",reg);
				}
				break;
			case TYPE_STRING:
				if(d->value){
					fprintf(f,".data\n");			
					symbol_code(d->symbol,reg);
					fprintf(f,"%s: .string ",reg);
					expr_get_string(d->value,f);
					fprintf(f,"\n");
				} else {
					fprintf(f,".data\n");
					symbol_code(d->symbol,reg);
					fprintf(f,"%s: .string \"\"\n",reg);
				}
				break;
			case TYPE_FUNCTION:
				if(d->code){
					symbol_code(d->symbol,reg);
					fprintf(f,".text\n.globl %s\n%s:\n",reg,reg);
					fprintf(f,"\t######## preamble of func ########\n");
					fprintf(f,"\n\tPUSHQ %%rbp\t# save the base pointer\n");
					fprintf(f,"\tMOVQ %%rsp, %%rbp\t# set the new base pointer to rsp\n");
					fprintf(f,"\n\tPUSHQ %%rdi\t# save first argument on the stack");
					fprintf(f,"\n\tPUSHQ %%rsi\t# save second argument on the stack");
					fprintf(f,"\n\tPUSHQ %%rdx\t# save third argument on the stack");
					fprintf(f,"\n\tPUSHQ %%rcx\t# save fourth argument on the stack");
					fprintf(f,"\n\tPUSHQ %%r8\t# save fifth argument on the stack");
					fprintf(f,"\n\tPUSHQ %%r9\t# save sixth argument on the stack\n");
					if(d->symbol->local_count != 0){
						fprintf(f,"\n\tSUBQ $%d, %%rsp\t# allocate space for local variables\n",8*d->symbol->local_count);
					}
					fprintf(f,"\n\tPUSHQ %%rbx\t# save callee-saved registers\n");
					fprintf(f,"\tPUSHQ %%r12\n");
					fprintf(f,"\tPUSHQ %%r13\n");
					fprintf(f,"\tPUSHQ %%r14\n");
					fprintf(f,"\tPUSHQ %%r15\n");

					fprintf(f,"\n\t######## body of the function starts here ########\n\n");

					stmt_codegen(d->code,f);

					fprintf(f,"\n\t######## epilouge of function restores the stack ########\n\n");
					
					fprintf(f,".RET%d:\n",return_count);
					return_count++;
	
					fprintf(f,"\tPOPQ %%r15\n");
					fprintf(f,"\tPOPQ %%r14\n");
					fprintf(f,"\tPOPQ %%r13\n");
					fprintf(f,"\tPOPQ %%r12\n");
					fprintf(f,"\tPOPQ %%rbx\n");

					fprintf(f,"\n\tMOVQ %%rbp, %%rsp\t# reset stack to base pointer\n");
					fprintf(f,"\tPOPQ %%rbp\t# restore the old base pointer\n");
					fprintf(f,"\n\tRET\t\t# return to caller\n");
					break;
				} else {
					// do nothing
				}
				break;
			default:
				printf("error: incorrect declaration type\n");
				exit(1);
				break;
		}
	} else if(d->symbol->kind == SYMBOL_LOCAL) {
		switch(d->type->kind){
			case TYPE_BOOLEAN:
			case TYPE_CHARACTER:
			case TYPE_INTEGER:
				if(d->value){
					// do nothing
				} else {
					// TO DO HERE
				}
				break;
			case TYPE_STRING:
				if(d->value){
					// do nothing?
				} else {
					// TO DO HERE
				}
				break;
			case TYPE_FUNCTION:
				// do nothing
				break;
			default:
				printf("error: incorrect declaration type\n");
				exit(1);
				break;
		}
	}

	decl_codegen(d->next,f);
}
