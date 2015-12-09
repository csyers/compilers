#include "token.h"
#include <string.h>

const char* token_string(token_t t){
	switch(t){
		case TOKEN_WHILE:
			return "WHILE";
			break;
		case TOKEN_STRING:
			return "STRING";
			break;
		case TOKEN_CHAR:
			return "CHAR";
			break;
		case TOKEN_INTEGER:
			return "INTEGER";
			break;
		case TOKEN_STRING_LITERAL:
			return "STRING_LITERAL";
			break;
		case TOKEN_CHARACTER_LITERAL:
			return "CHARACTER_LITERAL";
			break;
		case TOKEN_INTEGER_LITERAL:
			return "INTEGER_LITERAL";
			break;
		case TOKEN_IDENT:
			return "IDENTIFIER";
			break;
		case TOKEN_ARRAY:
			return "ARRAY";
			break;
		case TOKEN_BOOLEAN:
			return "BOOLEAN";
			break;
		case TOKEN_ELSE:
			return "ELSE";
			break;
		case TOKEN_FALSE:
			return "FALSE";
			break;
		case TOKEN_FOR:
			return "FOR";
			break;
		case TOKEN_FUNCTION:
			return "FUNCTION";
			break;
		case TOKEN_IF:
			return "IF";
			break;
		case TOKEN_PRINT:
			return "PRINT";
			break;
		case TOKEN_RETURN:
			return "RETURN";
			break;
		case TOKEN_TRUE:
			return "TRUE";
			break;
		case TOKEN_VOID:
			return "VOID";
			break;
		case TOKEN_ADD:
			return "ADD";
			break;
		case TOKEN_SUB:
			return "SUB";
			break;
		case TOKEN_MULT:
			return "MULT";
			break;
		case TOKEN_DIV:
			return "DIV";
			break;
		case TOKEN_MOD:
			return "MOD";
			break;
		case TOKEN_EXP:
			return "EXP";
			break;
		case TOKEN_INCREMENT:
			return "INCREMENT";
			break;
		case TOKEN_DECREMENT:
			return "DECREMENT";
			break;
		case TOKEN_GT:
			return "GT";
			break;
		case TOKEN_GE:
			return "GE";
			break;
		case TOKEN_LT:
			return "LT";
			break;
		case TOKEN_LE:
			return "LE";
			break;
		case TOKEN_EQ:
			return "EQ";
			break;
		case TOKEN_NE:
			return "NE";
			break;
		case TOKEN_ASSIGN:
			return "ASSIGN";
			break;
		case TOKEN_AND:
			return "AND";
			break;
		case TOKEN_OR:
			return "OR";
			break;
		case TOKEN_COLON:
			return "COLON";
			break;
		case TOKEN_SEMICOLON:
			return "SEMICOLON";
			break;
		case TOKEN_L_PAREN:
			return "L_PAREN";
			break;
		case TOKEN_R_PAREN:
			return "R_PAREN";
			break;
		case TOKEN_L_BRACE:
			return "L_BRACE";
			break;
		case TOKEN_R_BRACE:
			return "R_BRACE";
			break;
		case TOKEN_L_BRACKET:
			return "L_BRACKET";
			break;
		case TOKEN_R_BRACKET:
			return "R_BRACKET";
			break;
		case TOKEN_COMMA:
			return "COMMA";
			break;
		case TOKEN_LOGICAL_NOT:
			return "LOGICAL_NOT";
		// default cases - shouldn't reach here
		case TOKEN_ERROR:
			return "";
			break;
		default:
			return "scan error";
			break;
	}
}


void edit_string(char* word){
	int i, j;
	for(i = 1; i < strlen(word); i++){
		word[i-1] = word[i];
	}
	word[i-2] = '\0';			// remove second quote and end string there
	word[i-1] = '\0';
	for(i = 0; i < strlen(word); i++){
		if(word[i]=='\\'){
			if(word[i+1]=='n'){
				word[i] = '\n';
			} else if(word[i+1]=='0'){
				word[i] = '\0';	
				if(word[0] == '\0'){
					return;
				}
			} else{
				word[i] = word[i+1];
			}
			for(j = i+1; j < strlen(word)-1; j++){
				word[j] = word[j+1];
			}
			if(strlen(word) != 0){
				word[j] = '\0';
			}
		}
        }
}
