#include "expr.h"
#include "symbol.h"
#include "scope.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "register.h"

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
	e->reg = -1;
	
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
	e->reg = -1;	

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
	e->reg = -1;

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
	e->reg = -1;

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
	e->reg = -1;

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
	e->reg = -1;

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
			fflush(stdout);
			break;
		case EXPR_ASSIGN:
			if(L->kind == TYPE_FUNCTION){
				printf("type error: cannot assign ");
				type_print(e->left->symbol->type);
				printf(" (");
				expr_print(e->left);
				printf(") to a ");
				type_print(R);
				printf(" (");
				expr_print(e->right);
				printf(")\n");
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
				printf(")\n");
				error_count++;
				}				
			}
			return type_create(R->kind,0,0);
			break;
		case EXPR_ARRAY:
			printf("error: arrays not implemented\n");
			exit(1);
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
	if(!e) return 0;
	if(e->kind == EXPR_CHAR || e->kind == EXPR_STR || e->kind == EXPR_INT || e->kind == EXPR_BOOL){
		return 1;
	} else {
		return 0;
	}
}

void expr_codegen( struct expr *e, FILE* f)
{

	if(!e) return;
	int i;
	char reg_name[200];
	char arg_reg[6][100] = {"%rdi","%rsi","%rdx","%rcx","%r8","r%9"};
	struct expr *e_cursor;
	switch(e->kind){
		case EXPR_ADD:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tADD %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_SUB:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tSUB %s, %s\n",register_name(e->right->reg),register_name(e->left->reg));
			e->reg = e->left->reg;
			register_free(e->right->reg);
			break;
		case EXPR_MUL:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tMOV %s, %%rax\n",register_name(e->left->reg));
			fprintf(f,"\tIMUL %s\n",register_name(e->right->reg));
			fprintf(f,"\tMOV %%rax, %s\n",register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_DIV:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tMOV %s, %%rax\n",register_name(e->left->reg));
			fprintf(f,"\tCQTO\n");
			fprintf(f,"\tIDIV %s\n",register_name(e->right->reg));
			fprintf(f,"\tMOV %%rax, %s\n",register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_NAME:
			e->reg = register_alloc();
			symbol_code(e->symbol,reg_name);
			fprintf(f,"\tMOV %s, %s\n",reg_name,register_name(e->reg));
			break;
		case EXPR_BOOL:
		case EXPR_INT:
		case EXPR_CHAR:
			e->reg = register_alloc();
			fprintf(f,"\tMOV $%d, %s\n",e->literal_value,register_name(e->reg));
			break;
		case EXPR_STR:
			e->reg = register_alloc();
			fprintf(f,".data\n");
			fprintf(f,".STR%d: ",string_count);
			string_count++;
			fprintf(f,".string ");
			expr_get_string(e,f);
			fprintf(f,"\n");
			fprintf(f,".text\n");
			fprintf(f,"\tLEA .STR%d, %s\n",string_count-1,register_name(e->reg));
			break;
		case EXPR_OR:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tOR %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_AND:	
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tAND %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_LT:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tCMP %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			fprintf(f,"\tJLE .L%d\n",label_count);
			label_count++;
			fprintf(f,"\tMOV $1, %s\n",register_name(e->right->reg));
			fprintf(f,"\tJMP .L%d\n",label_count);
			label_count++;
			fprintf(f,".L%d:\n",label_count-2);
			fprintf(f,"\tMOV $0, %s\n",register_name(e->right->reg));
			fprintf(f,".L%d:\n",label_count-1);
			register_free(e->left->reg);
			e->reg = e->right->reg;
			break;
		case EXPR_GT:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tCMP %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			fprintf(f,"\tJGE .L%d\n",label_count);
			label_count++;
			fprintf(f,"\tMOV $1, %s\n",register_name(e->right->reg));
			fprintf(f,"\tJMP .L%d\n",label_count);
			label_count++;
			fprintf(f,".L%d:\n",label_count-2);
			fprintf(f,"\tMOV $0, %s\n",register_name(e->right->reg));
			fprintf(f,".L%d:\n",label_count-1);
			register_free(e->left->reg);
			e->reg = e->right->reg;
			break;
		case EXPR_LE:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tCMP %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			fprintf(f,"\tJL .L%d\n",label_count);
			label_count++;
			fprintf(f,"\tMOV $1, %s\n",register_name(e->right->reg));
			fprintf(f,"\tJMP .L%d\n",label_count);
			label_count++;
			fprintf(f,".L%d:\n",label_count-2);
			fprintf(f,"\tMOV $0, %s\n",register_name(e->right->reg));
			fprintf(f,".L%d:\n",label_count-1);
			register_free(e->left->reg);
			e->reg = e->right->reg;
			break;
		case EXPR_GE:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tCMP %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			fprintf(f,"\tJG .L%d\n",label_count);
			label_count++;
			fprintf(f,"\tMOV $1, %s\n",register_name(e->right->reg));
			fprintf(f,"\tJMP .L%d\n",label_count);
			label_count++;
			fprintf(f,".L%d:\n",label_count-2);
			fprintf(f,"\tMOV $0, %s\n",register_name(e->right->reg));
			fprintf(f,".L%d:\n",label_count-1);
			register_free(e->left->reg);
			e->reg = e->right->reg;
			break;
		case EXPR_EQ:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tCMP %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			fprintf(f,"\tJE .L%d\n",label_count);
			label_count++;
			fprintf(f,"\tMOV $0, %s\n",register_name(e->right->reg));
			fprintf(f,"\tJMP .L%d\n",label_count);
			label_count++;
			fprintf(f,".L%d:\n",label_count-2);
			fprintf(f,"\tMOV $1, %s\n",register_name(e->right->reg));
			fprintf(f,".L%d:\n",label_count-1);
			register_free(e->left->reg);
			e->reg = e->right->reg;
			break;
		case EXPR_NE:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tCMP %s, %s\n",register_name(e->left->reg),register_name(e->right->reg));
			fprintf(f,"\tJNE .L%d\n",label_count);
			label_count++;
			fprintf(f,"\tMOV $0, %s\n",register_name(e->right->reg));
			fprintf(f,"\tJMP .L%d\n",label_count);
			label_count++;
			fprintf(f,".L%d:\n",label_count-2);
			fprintf(f,"\tMOV $1, %s\n",register_name(e->right->reg));
			fprintf(f,".L%d:\n",label_count-1);
			register_free(e->left->reg);
			e->reg = e->right->reg;
			break;
		case EXPR_MOD:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tMOV %s, %%rax\n",register_name(e->left->reg));
			fprintf(f,"\tCQTO\n");
			fprintf(f,"\tIDIV %s\n",register_name(e->right->reg));
			fprintf(f,"\tMOV %%rdx, %s\n",register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_NEG:
			expr_codegen(e->right,f);
			fprintf(f,"\tNEG %s\n",register_name(e->right->reg));
			e->reg = e->right->reg;
			break;
		case EXPR_NOT:
			expr_codegen(e->right,f);
			fprintf(f,"\tXOR $1, %s\n",register_name(e->right->reg));
			e->reg = e->right->reg;
			break;
		case EXPR_EXP:
			expr_codegen(e->left,f);
			expr_codegen(e->right,f);
			fprintf(f,"\tPUSHQ %%r10\n");
			fprintf(f,"\tPUSHQ %%r11\n");
			fprintf(f,"\tMOV %s, %%rdi\n",register_name(e->left->reg));
			fprintf(f,"\tMOV %s, %%rsi\n",register_name(e->right->reg));
			fprintf(f,"\tCALL integer_power\n");
			fprintf(f,"\tPOPQ %%r11\n");
			fprintf(f,"\tPOPQ %%r10\n");
			fprintf(f,"\tMOV %%rax, %s\n",register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_INCR:
			expr_codegen(e->left,f);
			fprintf(f,"\tINC %s\n",register_name(e->left->reg));
			symbol_code(e->left->symbol,reg_name);
			fprintf(f,"\tMOV %s, %s\n",register_name(e->left->reg),reg_name);
			e->reg = e->left->reg;
			break;
		case EXPR_DECR:
			expr_codegen(e->left,f);
			fprintf(f,"\tDEC %s\n",register_name(e->left->reg));
			symbol_code(e->left->symbol,reg_name);
			fprintf(f,"\tMOV %s, %s\n",register_name(e->left->reg),reg_name);
			e->reg = e->left->reg;
			break;
		case EXPR_LIST:		// function calls, prints
			 e_cursor = e;
			 while(e_cursor->right->kind == EXPR_LIST){
			 	expr_codegen(e_cursor->left,f);
			 	fprintf(f, "\tMOV %s, %s\n", register_name(e_cursor->left->reg), arg_reg[argument_count]);
			 	argument_count++;
			 	register_free(e_cursor->left->reg);
			 	e_cursor = e_cursor->right;
			 } 
			 expr_codegen(e_cursor->left,f);
			 fprintf(f, "\tMOV %s, %s\n", register_name(e_cursor->left->reg), arg_reg[argument_count]);
			 argument_count++;
			 register_free(e_cursor->left->reg);
			 expr_codegen(e_cursor->right,f);
			 fprintf(f, "\tMOV %s, %s\n", register_name(e_cursor->right->reg), arg_reg[argument_count]);
			 argument_count++;
			 register_free(e_cursor->right->reg);
			 break;
		case EXPR_FUNC:		// function calls
			argument_count = 0;
			e->reg = register_alloc();
			fprintf(f,"\tPUSHQ %%r10\n");
			fprintf(f,"\tPUSHQ %%r11\n");
			if(e->right && e->right->kind != EXPR_LIST){
				expr_codegen(e->right,f);
				fprintf(f, "\tMOV %s, %%rdi\n", register_name(e->right->reg));
				register_free(e->right->reg);
				argument_count++;
			} else {
				expr_codegen(e->right,f);				
			}

			fprintf(f,"\tCALL %s\n",e->left->name);
			//for(i = argument_count-1; i >= 0; i--){
			//	fprintf(f,"\tPOPQ %s\n",arg_reg[i]);
			//}
			fprintf(f,"\tPOPQ %%r11\n");
			fprintf(f,"\tPOPQ %%r10\n");
			fprintf(f, "\tMOV %%rax, %s\n", register_name(e->reg));
			break;
		case EXPR_ARRAY:
			// do nothing (supposed to be arrays)
			break;
		case EXPR_ASSIGN:
			expr_codegen(e->right,f);
			symbol_code(e->left->symbol,reg_name);
			fprintf(f,"\tMOV %s, %s\n",register_name(e->right->reg),reg_name);
			e->reg = e->right->reg;
			break;
		case EXPR_BLOCK:
			// ???????????????????????????????????
			// do nothing (supposed to be arrays)
			break;
		case EXPR_GROUP:
			expr_codegen(e->left,f);
			e->reg = e->left->reg;
			break;
	}
}

void expr_get_string(struct expr *e, FILE* f)
{
	int i = 0;
	fprintf(f,"\"");
	for(i = 0; i < strlen(e->string_literal); i++){
		if(e->string_literal[i]==0){
			fprintf(f,"\\0");
		} else if(e->string_literal[i]==0x0A){
			fprintf(f,"\\n");
		} else {
			fprintf(f,"%c",e->string_literal[i]);
		}
	}
	fprintf(f,"\"");
}

void expr_print_codegen(struct expr *e, FILE *f)
{
	struct type *t;
	expr_codegen(e,f);
	t = expr_typecheck(e);
	char name[200];
	if(t->kind == TYPE_INTEGER){
		sprintf(name,"integer");
	} else if(t->kind == TYPE_BOOLEAN){
		sprintf(name,"boolean");
	} else if(t->kind == TYPE_CHARACTER) {
		sprintf(name,"character");
	} else if(t->kind == TYPE_STRING) {
		sprintf(name,"string");
	} else {
		printf("error: cannot print expression ");
		expr_print(e);
		printf("\n");
		exit(1);
	}
	fprintf(f,"\tPUSHQ %%r10\n");
	fprintf(f,"\tPUSHQ %%r11\n");
	fprintf(f,"\tMOV %s, %%rdi\n",register_name(e->reg));
	fprintf(f,"\tCALL print_%s\n",name);
	fprintf(f,"\tPOPQ %%r11\n");
	fprintf(f,"\tPOPQ %%r10\n");
	register_free(e->reg);
	
}
