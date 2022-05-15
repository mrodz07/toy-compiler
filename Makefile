CC_FLAGS = -Wall -Wextra -Wpedantic -lfl -lm

default: a.out

#run: test1 test2 test3 test4 test5 test6 test7 test8 test9

a.out: reconocedor.tab.c lex.yy.c
	cc $(CC_FLAGS) reconocedor.tab.c lex.yy.c

reconocedor.tab.c: reconocedor.y
	bison -d reconocedor.y

lex.yy.c: reconocedor.lex
	flex reconocedor.lex

.PHONY: clean run

clean:
	rm -rf *.o *.out lex.yy.c reconocedor.tab.c reconocedor.tab.h
