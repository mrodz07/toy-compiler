CC_FLAGS = -Wall -Wextra -Wpedantic -lfl

default: a.out

run: test1 test2 test3 test4 test5 test6 test7 test8 test9

test1: a.out
	./a.out prueba1.txt

test2: a.out
	./a.out prueba2.txt

test3: a.out
	./a.out prueba3.txt

test4: a.out
	./a.out prueba4.txt

test5: a.out
	./a.out prueba5.txt

test6: a.out
	./a.out prueba6.txt

test7: a.out
	./a.out prueba7.txt

test8: a.out
	./a.out prueba8.txt

test9: a.out
	./a.out prueba9.txt

a.out: lex.yy.c
	cc $(CC_FLAGS) lex.yy.c

lex.yy.c: reconocedor.lex
	flex reconocedor.lex

.PHONY: clean run

clean:
	rm -rf *.o *.out lex.yy.c
