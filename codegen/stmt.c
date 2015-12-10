#include "stmt.h"

struct stmt * stmt_create(stmt_kind_t kind, struct decl *d, struct expr *init_expr, struct expr *e, struct expr *next_expr, struct stmt *body, struct stmt *else_body )
{
	struct stmt *s = malloc(sizeof(*s));

	s->kind = kind;
	s->decl = d;
	s->init_expr = init_expr;
	s->expr = e;
	s->next_expr = next_expr;
	s->body = body;
	s->else_body = else_body;
	s->next = NULL;

	return s;
}

// recursively print the statements at a given indent
void stmt_print(struct stmt *s, int indent)
{
	if(!s) return;
	switch(s->kind){
		case STMT_EMPTY:
			break;
		case STMT_DECL:
			decl_print(s->decl,indent);
			break;
		case STMT_EXPR:
			printf("%*s",indent,"");
			expr_print(s->expr);
			printf(";");
			break;
		case STMT_IF_ELSE:
			printf("%*s",indent,"");
			printf("if(");
			expr_print(s->expr);
			printf(")\n");
			if(s->body->kind==STMT_BLOCK){
				stmt_print(s->body,indent);
			} else {
				stmt_print(s->body,indent+4);
			}
			if(s->else_body){
			printf("%*s",indent,"");
				printf("else\n");
				if(s->else_body->kind==STMT_BLOCK){
					stmt_print(s->else_body,indent);
				} else {
					stmt_print(s->else_body,indent+4);
				}
			}
			break;
		case STMT_FOR:
			printf("%*s",indent,"");
			printf("for(");
			expr_print(s->init_expr);
			printf(";");
			expr_print(s->expr);
			printf(";");
			expr_print(s->next_expr);
			printf(")\n");
			if(s->body->kind==STMT_BLOCK){
				stmt_print(s->body,indent);
			} else {
				stmt_print(s->body,indent+4);
			}
			break;
		case STMT_PRINT:
			printf("%*s",indent,"");
			printf("print ");
			expr_print(s->expr);
			printf(";");
			break;
		case STMT_RETURN:
			printf("%*s",indent,"");
			if(s->expr){
				printf("return ");
			} else {
				printf("return");
			}
			expr_print(s->expr);
			printf(";");
			break;
		case STMT_BLOCK:
			printf("%*s{\n",indent,"");
			stmt_print(s->body,indent+4);
			printf("%*s}",indent,"");
			break;
	}	
	if(s->kind!=STMT_DECL && s->kind!=STMT_IF_ELSE && s->kind!=STMT_FOR){
		printf("\n");
	}
	stmt_print(s->next,indent);
}


// function to resolve the statements in a cminor program
// @input: a statement in a list of statements
// @output: recursively calls resolve functions so that the error_count contains 
void stmt_resolve( struct stmt *s )
{
	if(!s) return;		// return if null
	switch(s->kind){		// switch on the kind of statement
		// empty statement (nothing happens)
		case STMT_EMPTY:
			break;
		// a statement that is a declaration resolve the decl using decl_resolve
		case STMT_DECL:
			decl_resolve(s->decl);
			break;
		// an if else statement: resolve the expr, body, and else_body
		case STMT_IF_ELSE:
			expr_resolve(s->expr);
			stmt_resolve(s->body);
			stmt_resolve(s->else_body);
			break;
		// a block of statements: enter a new scope (new HT) and resolve the statements inside of the block
		case STMT_BLOCK:
			scope_enter();
			stmt_resolve(s->body);
			scope_exit();
			break;
		// an expression: resolve the expression using expr_resolve
		case STMT_EXPR:
			expr_resolve(s->expr);
			break;
		// a for statement: resolve all parts of the for statements
		case STMT_FOR:
			expr_resolve(s->init_expr);
			expr_resolve(s->expr);
			expr_resolve(s->next_expr);
			stmt_resolve(s->body);
			break;
		// print statement: resolve the expression list 
		case STMT_PRINT:
			expr_resolve(s->expr);
			break;
		// return statement: resolve the expression
		case STMT_RETURN:
			expr_resolve(s->expr);
			break;
	}
	// resolve the next statement in the linked list
	stmt_resolve(s->next);
}

// function to typecheck a linked list of statements found in the function declaration d
// @inputs: linked list of statements s and the function delcaration they were found in, d
// @outputs: recursively typechecks the statements and all parts of the statements
void stmt_typecheck( struct stmt *s, struct decl *d )
{
	if(!s) return;			// base case
	struct type *T;			// a pointer to a type
	switch(s->kind){		// switch on the kind of statement being typechecked
		// if else statement: expr must be of type BOOLEAN, and typecheck the body and the else_body
		case STMT_IF_ELSE:		
			T = expr_typecheck(s->expr);
			if(T->kind != TYPE_BOOLEAN){
				printf("type error: if condition is of type ");
				type_print(T);
				printf(" (");
				expr_print(s->expr);
				printf("), where a boolean value is required\n");
				error_count++;
			}
			stmt_typecheck(s->body,d);
			stmt_typecheck(s->else_body,d);
			break;
		// declaration: cannot be a function declaration (outside of global scope), typecheck the decl
		case STMT_DECL: 
			if(s->decl->type->kind == TYPE_FUNCTION){
				printf("type error: cannot declare a function (%s) nested in another function (%s)\n",s->decl->name,d->name);
				error_count++;
			}
			decl_typecheck(s->decl);
			break;
		// expression: typecheck the expression using expr_typecheck
		case STMT_EXPR:
			expr_typecheck(s->expr);
			break;
		// for statement: check the init_expr, expr, next_expr, and body. expr must be of type boolean or empty (void)
		case STMT_FOR:
			expr_typecheck(s->init_expr);
			T = expr_typecheck(s->expr);
			if(T->kind != TYPE_BOOLEAN && T->kind != TYPE_VOID){
				printf("type error: for loop conditional is of type ");
				type_print(T);
				printf(" (");
				expr_print(s->expr);
				printf("), where a boolean value is required\n");
				error_count++;
			}
			expr_typecheck(s->next_expr);
			stmt_typecheck(s->body,d);
			break;
		// print: type check the expression being printed
		case STMT_PRINT:
			expr_typecheck(s->expr);
			break;
		// return: ensure the value being returned matches the subtype of the function it is called in (the return values must match)
		case STMT_RETURN:
			if(d->type->subtype->kind != expr_typecheck(s->expr)->kind){
				printf("type error: cannot return ");
				type_print(expr_typecheck(s->expr));
				printf(" in function (%s) that returns ",d->name);
				type_print(d->type->subtype);
				printf("\n");
				error_count++;
			}
			break;
		// block: typecheck the body of the block
		case STMT_BLOCK:
			stmt_typecheck(s->body,d);
			break;
		// empty: do nothing
		case STMT_EMPTY:
			// do nothing
			break;
	}
	// typecheck the next statement in the linked list
	stmt_typecheck(s->next,d);
}

void stmt_codegen (struct stmt *s, FILE* f) 
{
	int label_save1, label_save2;
	if(!s) return;
	struct expr *e;
	switch(s->kind){
		case STMT_DECL:
			decl_codegen(s->decl,f);
			break;
		case STMT_BLOCK:
			stmt_codegen(s->body,f);
			break;
		case STMT_EXPR:
			expr_codegen(s->expr,f);
			register_free(s->expr->reg);
			break;
		case STMT_IF_ELSE:
			fprintf(f,"\n\t#### if statement ####\n");
			expr_codegen(s->expr,f);
			fprintf(f,"\tCMP $0, %s\n",register_name(s->expr->reg));
			register_free(s->expr->reg);
			fprintf(f,"\tJE .L%d\n",label_count);
			label_save1 = label_count;
			label_count++;
			stmt_codegen(s->body,f);
			fprintf(f,"\tJMP .L%d\n",label_count);
			label_save2 = label_count;
			label_count++;
			fprintf(f,".L%d:\n",label_save1);
			stmt_codegen(s->else_body,f);
			fprintf(f,".L%d:\n",label_save2);
			break;
		case STMT_FOR:
			fprintf(f,"\n\t#### for loop ####\n");
			expr_codegen(s->init_expr,f);
			register_free(s->init_expr->reg);
			fprintf(f,".L%d:\n",label_count);
			label_save1 = label_count;
			label_count++;
			expr_codegen(s->expr,f);
			register_free(s->expr->reg);
			fprintf(f,"\tCMP $0, %s\n",register_name(s->expr->reg));
			fprintf(f,"\tJE .L%d\n",label_count);
			label_save2 = label_count;
			label_count++;	
			stmt_codegen(s->body,f);
			expr_codegen(s->next_expr,f);
			register_free(s->next_expr->reg);
			fprintf(f,"\tJMP .L%d\n",label_save1);
			fprintf(f,".L%d:\n",label_save2);
			break;
		case STMT_PRINT:
			if(s->expr){
				if(s->expr->kind != EXPR_LIST){				// ONLY 1 ARGUMENT
					expr_print_codegen(s->expr,f);
				} else {
					e = s->expr;					// MORE THAN 1 ARGUMENT
					while(e->right->kind == EXPR_LIST){		
						expr_print_codegen(e->left,f);		// 
						e = e->right;
					}
					expr_print_codegen(e->left,f);
					expr_print_codegen(e->right,f);
				}
			} else {
			}
			break;
		case STMT_RETURN:
			if(s->expr){
				expr_codegen(s->expr,f);
				fprintf(f,"\tMOV %s, %%rax\n",register_name(s->expr->reg));
				fprintf(f,"\tJMP .RET%d\n",return_count);
				register_free(s->expr->reg);
				break;
			} else {
				fprintf(f,"\tJMP .RET%d\n",return_count);	
			}
	}
	stmt_codegen(s->next,f);
}
