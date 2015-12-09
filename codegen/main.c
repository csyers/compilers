#include "token.h"
#include "parser.tab.h"
#include "decl.h"
#include "stmt.h"
#include "param_list.h"
#include "expr.h"
#include "type.h"
#include "symbol.h"
#include "hash_table.h"
#include "scope.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
void print_usage_and_exit();
void scan(char*);
void print(char*);
void resolve(char*);
void typecheck(char*);
void codegen(char*,char*);
extern int yyparse();
extern int yylex();
extern const char* token_string(token_t t);
extern FILE* yyin;		// file to read from
extern char* yytext;		// holds matches
extern struct decl* program;
//extern int error_count;
//extern struct hash_table *h;

int error_count;
struct hash_table *h;
int reg[16] = {1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0};
int label_count = 0;
int string_count = 0;
int return_count = 0;

int main(int argc, char* argv[]){
	int opt, long_index;
	static struct option long_options[] = {
		{"scan",	required_argument, 0, 's'},
		{"print",	required_argument, 0, 'p'},
		{"resolve",	required_argument, 0, 'r'},
		{"typecheck",	required_argument, 0, 't'},
                {"codegen",	required_argument, 0, 'c'},
		{0,		0,		   0, 0}
	};
	while ((opt = getopt_long_only(argc, argv,"",long_options,&long_index)) != -1){		// go through the command line options
		switch(opt){
			case 's':			// if the option was -scan
				scan(optarg);			// scan the file argument
				break;
			case 'p':			// if the option was -print
				print(optarg);			// parse into AST and print 
				break;
			case 'r':			// if the option was -resolve
				resolve(optarg);		// parse, resolve, and display the error count
				break;
			case 't':			// if the option was -typecheck
				typecheck(optarg);		// parse, resolve, and typecheck to display the total errors
				break;	
			case 'c':
				if(argv[3]==NULL){
					print_usage_and_exit();
				}
				codegen(optarg,argv[3]);	// if the option was -codegen, generate the code
				break;
			default:
				print_usage_and_exit();	// if the wrong options were given
		}

	}
	return 0;
}

void print_usage_and_exit(){
  	printf("Usage: ./cminor -[scan/print/resolve/typecheck/codegen] sourcefile.cmnior [outfile.s]\n");
  	exit(1);
}

// scan the file passed in. Works the same as the scanner potion of the compiler project. Will print out the stream of tokens
// read from the input file
void scan(char *file){
	int t;
	yyin = fopen(file,"r");
	if(!yyin){
        	printf("Could not open file '%s'\n",file);
	 	exit(1);
        }
	while(1){
		t = yylex();
		if (!t){
			break;
		} else{				// if t is not NULL and not 0
			if(t==TOKEN_STRING_LITERAL || t==TOKEN_CHARACTER_LITERAL){	// if it requires a literal,
				printf("%s %s\n",token_string(t),yytext);				// print out the string and the value
			} else {
				printf("%s\n",token_string(t));						// print out the string of the token
			}
		}
	}
	return;
}


// parse the file passed in and prints out the syntax tree. Will fail with exit code 1 if the parse is unsuccessful
void print(char *file){
	h = hash_table_create(0,0);
	yyin = fopen(file,"r");
	if(!yyin){
        	printf("Could not open file '%s'\n",file);
	 	exit(1);
        }
	if(yyparse()==0){
		decl_print(program,0);		// recursively print the syntax tree
	} else {
		printf("parse failed\n");
		exit(1);
	}
	return;
}

// resolve the AST after creating it with parse
void resolve(char *file){
	h = hash_table_create(0,0);
	yyin = fopen(file,"r");
	if(!yyin){
        	printf("Could not open file '%s'\n",file);
	 	exit(1);
        }
	if(yyparse()==0){			// parse the file - will exit if there are parse errors
		decl_resolve(program);		// resolves recursively starting with the first decl
		if(error_count!=0){	
			printf("resolve error count: %d\n",error_count);	// displays the total number of errors
			exit(1);						// exit with status 1 if there were any resolve errors
		}
	} else {			
		exit(1);			// exit with status 1 if there were parse errors
	}
	return;			
}

// typecheck a cminor file for type errors
void typecheck(char *file){
	h = hash_table_create(0,0);		// intial hash table is empty
	yyin = fopen(file,"r");	
	if(!yyin){
        	printf("Could not open file '%s'\n",file);
	 	exit(1);						// exit 1 if file is not readable
        }
	if(yyparse()==0){						// parse the file
		decl_resolve(program);					// resolve recursively starting with the first decl
		if(error_count!=0){					
			printf("resolve error count: %d\n",error_count);// print the resolve error counts
			exit(1);					// exit with status 1 if there were resolve errors
		}
		decl_typecheck(program);				// recursively typecheck the program
		printf("typecheck error count: %d\n",error_count);	// print the total number of typecheck errors
		if(error_count!=0){					
			exit(1);					// exit 1 if there were typecheck errors
		}
	} else {
		exit(1);						// exit 1 if there were parse errors
	}
	return;
}

void codegen(char *file, char *outfile){
	int i;
	h = hash_table_create(0,0);		// intial hash table is empty
	yyin = fopen(file,"r");	
	if(!yyin){
        	printf("Could not open file '%s'\n",file);
	 	exit(1);						// exit 1 if file is not readable
        }
	if(yyparse()==0){						// parse the file
		decl_resolve(program);					// resolve recursively starting with the first decl
		if(error_count!=0){					
			printf("resolve error count: %d\n",error_count);// print the resolve error counts
			exit(1);					// exit with status 1 if there were resolve errors
		}
		decl_typecheck(program);				// recursively typecheck the program
		if(error_count!=0){					
			printf("typecheck error count: %d\n",error_count);	// print the total number of typecheck errors
			exit(1);					// exit 1 if there were typecheck errors
		}
		FILE* f = fopen(outfile,"w");
		if(f==NULL){
			printf("error: cannot open file (%s) for writing\n",outfile);
			exit(1);
		}
		decl_codegen(program,f);
		
		for(i = 0; i < 16; i++){
			printf("reg[%d]\t%d",i,reg[i]);
			printf("\n");
		}
	} else {
		exit(1);						// exit 1 if there were parse errors
	}
	return;
}
