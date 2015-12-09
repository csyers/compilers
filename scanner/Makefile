CC=gcc

all: cminor

cminor: lex.yy.c token.c main.c scanner.l
	$(CC) lex.yy.c token.c main.c -o cminor 

lex.yy.c: scanner.l
	flex scanner.l

.PHONY: clean
clean:
	-rm lex.yy.c cminor
