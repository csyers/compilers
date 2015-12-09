#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

void print_usage_and_exit();
void scan(char*);

int main(int argc, char* argv[]){
	int opt, long_index;
	static struct option long_options[] = {
		{"scan",	required_argument, 0, 's'},
		{0,		0,		   0, 0}
	};
	while ((opt = getopt_long_only(argc, argv,"",long_options,&long_index)) != -1){
		switch(opt){
			case 's':
				scan(optarg);
				break;
			default:
				print_usage_and_exit();
		}

	}
	return 0;
}

void print_usage_and_exit(){
  	printf("Usage: ./cminor -scan sourcefile.cmnior\n");
  	exit(1);
}

void scan(char *file){
        extern FILE* yyin;		// file to read from
	extern char* yytext;		// holds matches
	int t, c;
	yyin = fopen(file,"r");
	if(!yyin){
        	printf("Could not open file '%s'\n",file);
	 	exit(1);
        }
	while(1){
		t = yylex();
		if (!t){
			break;
		} else {				// if t is not NULL and not 0
			if(t==TOKEN_STRING_LITERAL || t==TOKEN_CHARACTER_LITERAL){	// if it requires a literal,
				printf("%s %s\n",token_string(t),yytext);		// print out the string and the value
			} else { 							// or if it is a normal token
				printf("%s\n",token_string(t));				// print out the string of the token
			}
		}
		
	}
	return;
}
