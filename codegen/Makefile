CC=gcc

all: cminor program

program: out.s library.o
	$(CC) out.s library.o -o program

out.s: test.cminor
	./cminor -codegen test.cminor out.s

cminor: main.o decl.o param_list.o stmt.o expr.o type.o scanner.o parser.tab.o token.o symbol.o hash_table.o scope.o register.o
	$(CC) -g -O0 main.o decl.o param_list.o stmt.o expr.o type.o scanner.o parser.tab.o token.o symbol.o scope.o hash_table.o register.o -o cminor

%.o: %.c *.h parser.tab.h
	$(CC) -c $< -o $@

scanner.c:  scanner.l parser.tab.h 
	flex -o scanner.c scanner.l

parser.tab.c parser.tab.h: cminor.y
	yacc -d -bparser -v cminor.y

.PHONY: clean
clean:
	-rm -f parser.tab.* parser.output scanner.c cminor *.o out.s a.out scanner program
