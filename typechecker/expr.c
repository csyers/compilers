#include "expr.h"
#include "symbol.h"
#include "scope.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int error_count;

struct expr * expr_create(expr_t kind, struct expr *left, struct expr *right )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = kind;
	e->left = left;
	e->right = right;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = 0;
	e->string_literal = NULL;

	return e;
}

struct expr * expr_create_name( const char *n)
{
	struct expr *e = malloc(sizeof(*e));
	
	e->kind = EXPR_NAME;
	e->left = NULL;
	e->right = NULL;
	e->name = n;
	e->symbol = NULL;
	e->literal_value = 0;
	e->string_literal = NULL;
	
	return e;
}

struct expr * expr_create_boolean_literal( int c )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = EXPR_BOOL;
	e->left = NULL;
	e->right = NULL;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = c;
	e->string_literal = NULL;

	return e;
}

struct expr * expr_create_integer_literal( int c )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = EXPR_INT;
	e->left = NULL;
	e->right = NULL;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = c;
	e->string_literal = NULL;

	return e;
}

struct expr * expr_create_character_literal( int c )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = EXPR_CHAR;
	e->left = NULL;
	e->right = NULL;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = c;
	e->string_literal = NULL;

	return e;
}

struct expr * expr_create_string_literal( const char *str )
{
	struct expr *e = malloc(sizeof(*e));

	e->kind = EXPR_STR;
	e->left = NULL;
	e->right = NULL;
	e->name = NULL;
	e->symbol = NULL;
	e->literal_value = 0;
	e->string_literal = str;

	return e;
}

// recursively print out the expression
void expr_print(struct expr *e)
{
	int i;
	if(!e) return;
	if(e->kind==EXPR_GROUP){
		printf("(");
	} else if (e->kind==EXPR_BLOCK){
		printf("{");
	}
	expr_print(e->left);
	switch(e->kind){
		case EXPR_ADD:
			printf("+");	
			break;
		case EXPR_SUB:
			printf("-");	
			break;
		case EXPR_MUL:
			printf("*");	
			break;
		case EXPR_DIV:
			printf("/");	
			break;
		case EXPR_NAME:
			printf("%s",e->name);	
			break;
		case EXPR_BOOL:
			if(e->literal_value){
				printf("true");
			} else {
				printf("false");
			}	
			break;
		case EXPR_INT:
			printf("%d",e->literal_value);	
			break;
		case EXPR_CHAR:
			if(e->literal_value==0){
				printf("\'\\0\'");
			} else if(e->literal_value==0x0A){
				printf("\'\\n\'");	
			} else {
				printf("\'%c\'",e->literal_value);	
			}
			break;
		case EXPR_STR:
			printf("\"");
			for(i = 0; i < strlen(e->string_literal); i++){
				if(e->string_literal[i]==0){
					printf("\\0");
				} else if(e->string_literal[i]==0x0A){
					printf("\\n");
				} else {
					printf("%c",e->string_literal[i]);
				}
			}
			printf("\"");
			break;
		case EXPR_OR:
			printf("||");	
			break;
		case EXPR_AND:
			printf("&&");	
			break;
		case EXPR_LT:
			printf("<");	
			break;
		case EXPR_GT:
			printf(">");	
			break;
		case EXPR_LE:
			printf("<=");	
			break;
		case EXPR_GE:
			printf(">=");	
			break;
		case EXPR_EQ:
			printf("==");	
			break;
		case EXPR_NE:
			printf("!=");	
			break;
		case EXPR_MOD:
			printf("%c",'%');	
			break;
		case EXPR_NEG:
			printf("-");	
			break;
		case EXPR_NOT:
			printf("!");	
			break;
		case EXPR_EXP:
			printf("^");	
			break;
		case EXPR_INCR:
			printf("++");	
			break;
		case EXPR_DECR:
			printf("--");	
			break;
		case EXPR_LIST:
			printf(",");	
			break;
		case EXPR_FUNC:
			printf("(");
			break;
		case EXPR_ARRAY:
			printf("[");
			break;
		case EXPR_ASSIGN:
			printf("=");
			break;
		case EXPR_BLOCK:
			printf("}");
			break;
//		case EXPR_ARRAY_LIST:
//			printf("][");
//			break;
	}
	expr_print(e->right);
	if(e->kind==EXPR_FUNC){
		printf(")");
	} else if (e->kind==EXPR_ARRAY){
		printf("]");
	}
	if(e->kind==EXPR_GROUP){
		printf(")");
	}
}

// resolve an expression tree recursively
// @input: an expression pointer e
// @return: error_count is updated to reflect the nubmer of resolve errors
void expr_resolve ( struct expr *e )
{
	if(!e) return;			// base case
	
	// resolve the left and right children of the expression
	expr_resolve(e->left);
	expr_resolve(e->right);

	// if the expression is a name
	if(e->kind == EXPR_NAME)
	{
		struct symbol *s = scope_lookup_multiple(e->name);		// look up 
		if(s) {
			e->symbol = s;
			switch (s->kind){
				case SYMBOL_LOCAL:
					printf("%s resolves to local %d\n",e->name,s->which);
					break;
				case SYMBOL_GLOBAL:
					printf("%s resolves to global %s\n",e->name,s->name);
					break;
				case SYMBOL_PARAM:
					printf("%s resolves to param %d\n",e->name,s->which);
					break;
			}
		} else {
			printf("resolve error: %s is not defined\n",e->name);
			++error_count;
		}
	}
}

struct type * expr_typecheck( struct expr *e ) 
{
	if(!e) return type_create(TYPE_VOID,0,0);

	struct type *L = expr_typecheck(e->left);
	struct type *R = expr_typecheck(e->right);
	struct type *arg;
	struct param_list *param_cursor;
	struct symbol *sym;
	struct expr *expr_cursor;
	int arg_count;
	int param_count;
	int counter=0;
	switch(e->kind){
		case EXPR_INT:
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_NAME:
			return e->symbol->type;
			break;
		case EXPR_FUNC:
			//check the param list versus the
			sym = scope_lookup_multiple(e->left->name);
			expr_cursor = e->right;
			param_cursor = sym->type->params;
			// counter variables for number of params and arguments to make sure they match
			param_count = 0;
			arg_count = 0;
			// count the number of expressions in the expression list
			while(expr_cursor){
				if(expr_cursor->kind==EXPR_LIST){
					if(expr_cursor->right->kind!=EXPR_LIST) {
						arg_count++;
						arg_count++;
						break;
					} else {
						arg_count++;
						expr_cursor = expr_cursor->right;
					}
				} else {
					arg_count++;
					break;
				}
			}
			// count the nubmer of parameters in the parameter list
			while(param_cursor){
				param_count++;
				param_cursor = param_cursor->next;
			}
			// if the counts are not the same, there is no way the function calls match, print an error
			if(param_count != arg_count){
				printf("type error: function %s requires %d arguments, where %d passed in\n",e->left->name,param_count,arg_count);
				error_count++;
				return type_create(sym->type->subtype->kind,0,0);
			}
			// set the cursors to the first element in the param list
			expr_cursor = e->right;
			param_cursor = sym->type->params;
			// if there is only 1 argument, compare the right child of the EXPR_FUNC and the type of the param_cursor
			if(arg_count==1){
				if(!type_compare(expr_typecheck(e->right),param_cursor->type)){
					printf("type error: argument 1 of function %s is of type ",e->left->name);
					type_print(expr_typecheck(e->right));
					printf(" (");
					expr_print(e->right);
					printf(") when a ");
					type_print(param_cursor->type);
					printf(" is required\n");
					error_count++;
				}
			} else {
				while (expr_cursor){
					counter++;		
					// case there are more expr_lists down the right side, not at last 2	
					if(expr_cursor->right->kind == EXPR_LIST){
						if(!type_compare(expr_typecheck(expr_cursor->left),param_cursor->type)){
							printf("type error: argument %d of function %s is of type ",counter,e->left->name);
							type_print(expr_typecheck(expr_cursor->left));
							printf(" (");
							expr_print(expr_cursor->left);
							printf(") when a ");
							type_print(param_cursor->type);
							printf(" is required\n");
							error_count++;
						}
					// case where the right child is not of kind EXPR_LIST (at last 2 arguments)
					} else {
						// if left expr matches second to last param
						if(!type_compare(expr_typecheck(expr_cursor->left),param_cursor->type)){
							printf("type error: argument %d of function %s is of type ",counter,e->left->name);
							type_print(expr_typecheck(expr_cursor->left));
							printf(" (");
							expr_print(expr_cursor->left);
							printf(") when a ");
							type_print(param_cursor->type);
							printf(" is required\n");
							error_count++;
						}
						// if right expr matches last param
						param_cursor = param_cursor->next;
						if(!type_compare(expr_typecheck(expr_cursor->right),param_cursor->type)){
							printf("type error: argument %d of function %s is of type ",counter,e->left->name);
							type_print(expr_typecheck(expr_cursor->right));
							printf(" (");
							expr_print(expr_cursor->right);
							printf(") when a ");
							type_print(param_cursor->type);
							printf(" is required\n");
							error_count++;
						}
						expr_cursor = 0;
					}
				if(expr_cursor){
					expr_cursor = expr_cursor->right;
				}
				param_cursor = param_cursor->next;
				}
			}
			return type_create(sym->type->subtype->kind,0,0);
			break;
		case EXPR_BOOL:
			return type_create(TYPE_BOOLEAN,0,0);
			break;
		case EXPR_CHAR:
			return type_create(TYPE_CHARACTER,0,0);
			break;
		case EXPR_STR:
			return type_create(TYPE_STRING,0,0);
			break;
		case EXPR_ADD:
			if(L->kind != TYPE_INTEGER || R->kind != TYPE_INTEGER) {
				printf("type error: cannot add ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_SUB:
			if(L->kind != TYPE_INTEGER || R->kind != TYPE_INTEGER) {
				printf("type error: cannot subtract ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_MUL:
			if(L->kind != TYPE_INTEGER || R->kind != TYPE_INTEGER) {
				printf("type error: cannot multiply ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_DIV:
			if(L->kind != TYPE_INTEGER || R->kind != TYPE_INTEGER) {
				printf("type error: cannot divide ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_OR:
			printf("L kind = %d, R kind = %d, boolean is type %d\n",L->kind,R->kind,TYPE_BOOLEAN);
			if(L->kind != TYPE_BOOLEAN || R->kind != TYPE_BOOLEAN) {
				printf("type error: cannot use logical or on ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_BOOLEAN,0,0);
			break;
		case EXPR_AND:
			printf("L kind = %d, R kind = %d, boolean is type %d\n",L->kind,R->kind,TYPE_BOOLEAN);
			if(L->kind != TYPE_BOOLEAN || R->kind != TYPE_BOOLEAN) {
				printf("type error: cannot use logical and on ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_BOOLEAN,0,0);
			break;
		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
			if(L->kind != TYPE_INTEGER || R->kind != TYPE_INTEGER) {
				printf("type error: cannot compare ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_BOOLEAN,0,0);
			break;
		case EXPR_EQ:
		case EXPR_NE:
	/*		if(e->left->kind == EXPR_ARRAY || e->left->kind == EXPR_FUNC || e->right->kind == EXPR_ARRAY || e->right->kind == EXPR_FUNC){
				printf("type error: cannot compare ");
				fflush(stdout);
				type_print(e->left->symbol->type);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(e->right->symbol->type);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			} else {
	*/			
				if(!type_compare(L,R)){
					printf("type error: cannot compare ");
					type_print(L);
					printf(" (");
					expr_print(e->left);
					printf(") and ");
					type_print(R);
					printf(" (");
					expr_print(e->right);
					printf(")\n");
					error_count++; 
				}
	//		}
			return type_create(TYPE_BOOLEAN,0,0);
			break;
		case EXPR_MOD:
			if(L->kind != TYPE_INTEGER || R->kind != TYPE_INTEGER) {
				printf("type error: cannot mod ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_NEG:
			if(R->kind != TYPE_INTEGER){
				printf("type error: cannot use unary minus on ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_NOT:
			if(R->kind != TYPE_BOOLEAN){
				printf("type error: cannot use logical not on ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++;
			}
			return type_create(TYPE_BOOLEAN,0,0);
			break;
		case EXPR_EXP:
			if(R->kind != TYPE_INTEGER || L->kind != TYPE_INTEGER){
				printf("type error: cannot use exponent on ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") and ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_INCR:
			if(L->kind != TYPE_INTEGER){
				printf("type error: cannot increment ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_DECR:
			if(L->kind != TYPE_INTEGER){
				printf("type error: cannot decrement ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(")\n");
				error_count++; 
			}
			return type_create(TYPE_INTEGER,0,0);
			break;
		case EXPR_ASSIGN:
			if(e->left->symbol->type->kind == TYPE_FUNCTION){
				printf("type error: cannot assign ");
				type_print(e->left->symbol->type);
				printf(" (");
				expr_print(e->left);
				printf(") to a ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf("(\n");
				error_count++;
			} else {
				if(!type_compare(L,R)){
				printf("type error: cannot assign ");
				type_print(L);
				printf(" (");
				expr_print(e->left);
				printf(") to a ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf("(\n");
				error_count++;
				}
			}
			return type_create(R->kind,0,0);
			break;
		case EXPR_ARRAY:
			if(R->kind != TYPE_INTEGER){
				printf("type error: cannot index array with ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
				error_count++;
			}
			return type_create(e->left->symbol->type->subtype->kind,0,0);
			break;
		case EXPR_BLOCK:
			expr_cursor = e->left;
			while(expr_cursor){
				if(expr_cursor->kind==EXPR_LIST){
					if(expr_cursor->right->kind != EXPR_LIST){
						counter += 2;
						break;
					} else {
						counter++;
						expr_cursor = expr_cursor->right;
					}
				} else {
					counter++;
					break;
				}
			}
			return type_create_array(TYPE_ARRAY,0,expr_typecheck(e->left->left),expr_create_integer_literal(counter));
			break;
		case EXPR_GROUP:
			return type_create(L->kind,0,0);	// what is a group
			break;
		case EXPR_LIST:
			return type_create(TYPE_ARRAY,0,0);
			break;
	}
}

// FUNCTION NOT USED
struct expr * expr_copy(struct expr *e)
{
	if(!e) return NULL;

	struct expr *copy = malloc(sizeof(*e));
	char *name_dup, *string_dup;
	if(e->name){
		name_dup = strdup(e->name);
	} 
	if(e->string_literal){
		string_dup = strdup(e->string_literal);
	}
	switch(e->kind){
		case EXPR_STR:
			return expr_create_string_literal(string_dup);
			break;
		case EXPR_NAME:
			copy =  expr_create_name(name_dup);
			copy->symbol = scope_lookup_multiple(e->name);
			break;
		case EXPR_BOOL:
			return expr_create_boolean_literal(e->literal_value);
			break;
		case EXPR_INT:
			return expr_create_integer_literal(e->literal_value);
			break;
		case EXPR_CHAR:
			return expr_create_character_literal(e->literal_value);
			break;
		default:
			expr_create(e->kind,expr_copy(e->left),expr_copy(e->right));
			break;
	}
	
}

// compare two expr pointers, return 0 if not the same, and 1 if the same 
// @input: expr *a, expr *b, two expr pointers to compare
// @return: 0 if not the same, 1 if they are the same
int expr_compare(struct expr *a, struct expr *b)
{
	if(!a && !b) return 1;		// if both are null, return true
	if(!a || !b) return 0;		// if one is null and the other isn't, return 0

	// if there both have names, compare all elements including the names
	if(a->name && b->name){	
		return (a->kind==b->kind && expr_compare(a->left,b->left) && expr_compare(a->right,b->right) && strcmp(a->name,b->name)==0 && a->literal_value==b->literal_value);	
	// if neither have names, go in here
	} else if (!a->name && !b->name){
		// if both have string literals, compare everything except the names
		if(a->string_literal && b->string_literal) {
			return (a->kind==b->kind && expr_compare(a->left,b->left) && expr_compare(a->right,b->right) && strcmp(a->string_literal,b->string_literal)==0 && a->literal_value==b->literal_value);	
		// if neither have string_literals, then compare everything except names and string literals
		} else if (!a->string_literal && !b->string_literal) {
			return (a->kind==b->kind && expr_compare(a->left,b->left) && expr_compare(a->right,b->right) && a->literal_value==b->literal_value);
		// if one has a string literal and the other doesn't, they are not the same
		} else {
			return 0;
		}
	// if one has a name and the other doesn't, they are not the same
	} else {
		return 0;
	}
}

// recursively determines if a expr AST contains any name, used for
// @input: expr pointer that is the head of a tree to search
// @return: 1 if there no names in the expression tree, 0 if there is a name
int expr_is_constant( struct expr *e )
{
	if(!e) return 1;
	if(e->name){
		return 0;
	}
	return expr_is_constant(e->left) && expr_is_constant(e->right);
}
