CC_FLAGS = -Wall -Wextra -Wpedantic -lfl -lm
BISON_FLAGS = -d -Wcounterexamples

default: a.out

test_all: test1 test2 test3 test4 test5 test6 test7 test8

test1: default
	./a.out tests/prog1.txt

test2: default
	./a.out tests/prog2.txt

test3: default
	./a.out tests/prog3.txt

test4: default
	./a.out tests/prog4.txt

test5: default
	./a.out tests/prog5.txt

test6: default
	./a.out tests/prog6.txt

test7: default
	./a.out tests/prog7.txt

test8: default
	./a.out tests/prog8.txt

a.out: reconocedor.tab.c lex.yy.c
	cc $(CC_FLAGS) reconocedor.tab.c lex.yy.c

reconocedor.tab.c: reconocedor.y
	bison $(BISON_FLAGS) reconocedor.y

lex.yy.c: reconocedor.lex
	flex reconocedor.lex

.PHONY: clean run

clean:
	rm -rf *.o *.out lex.yy.c reconocedor.tab.c reconocedor.tab.h
