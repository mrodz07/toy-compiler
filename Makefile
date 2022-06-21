CC_FLAGS = -Wall -Wextra -Wpedantic -lfl -lm -g
BISON_FLAGS = -d -Wcounterexamples

default: output

test_all: test1 test2 test3 test4 test5 test6 test7 test8

test1: default
	./output tests/prog1.txt

test2: default
	./output tests/prog2.txt

test3: default
	./output tests/prog3.txt

test4: default
	./output tests/prog4.txt

test5: default
	./output tests/prog5.txt

test6: default
	./output tests/prog6.txt

test7: default
	./output tests/prog7.txt

test8: default
	./output tests/prog8.txt

output: parser.tab.c lex.yy.c
	cc $(CC_FLAGS) parser.tab.c lex.yy.c -o output

parser.tab.c: parser.y
	bison $(BISON_FLAGS) parser.y

lex.yy.c: lexer.lex functions.c functions.h
	flex lexer.lex

.PHONY: clean run

clean:
	rm -rf output *.out lex.yy.c *.tab.c *.tab.h
