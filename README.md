miniC (using flex&bison)

miniC compiler to pdl for model checking purposes
========
This project is compiler project using flex and bison.

## 1. Building
    make
    
    or

    flex miniC.l
    bison -d miniC.y
    gcc -o miniC miniC.tab.c print.c symboltable.c lex.yy.c -g -lfl
    
## 2. How to run
    miniC < input.txt

## 3. Results
    Abstract Syntax Tree (AST): tree.txt
    Symbol table: table.txt
