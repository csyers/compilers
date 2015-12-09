CC=gcc

all: cminor

cminor: main.o decl.o param_list.o stmt.o expr.o type.o scanner.o parser.tab.o token.o symbol.o
	$(CC) -g -O0 main.o decl.o param_list.o stmt.o expr.o type.o scanner.o parser.tab.o token.o symbol.o -o cminor

%.o: %.c *.h parser.tab.h
	gcc -c $< -o $@

scanner.c:  scanner.l parser.tab.h 
	flex -o scanner.c scanner.l

parser.tab.c parser.tab.h: cminor.y
	yacc -d -bparser -v cminor.y

.PHONY: clean
clean:
	-rm -f parser.tab.* parser.output scanner.c cminor *.o 
