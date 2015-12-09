#include "stmt.h"

#include <stdio.h>
#include <stdlib.h>


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

void stmt_print(struct stmt *s, int indent)
{
	if(!s) return;
	switch(s->kind){
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
