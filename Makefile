all:
	flex miniC.l
	bison -d miniC.y
	gcc -o miniC miniC.tab.c print.c symboltable.c lex.yy.c Converter.c PDLtoNuX.c -g -lfl

clean:
	rm -rf lex.yy.c miniC.tab.c miniC.tab.h miniC tree.txt table.txt
