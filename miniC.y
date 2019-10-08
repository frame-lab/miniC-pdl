%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "AST.h"
#include "print.h"
#include "lex.yy.h"
#include "Converter.h"
#include "symboltable.h"
//global variables which can be used in other .c .h
struct PROGRAM *head;
FILE *fp;   //for AST
FILE *fp2;  //for symboltable
int option[7];
int pos[3] = {-5, -5, -5};
int range[2];
char *pdl;
char *smv;
void yyerror(char const* text) {

    fprintf(stderr, "%s\n", text);
}
/*
void lyyerror(YYLTYPE t, char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    if(t.first_line)
        fprintf(stderr, "%d.%d-%d.%d: error: ", t.first_line, t.first_column, t.last_line, t.last_column);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}
*/
%}

%union{
    struct PROGRAM       *ptr_program;
    struct DECLARATION   *ptr_declaration;
    struct IDENTIFIER    *ptr_identifier;
    struct FUNCTION      *ptr_function;
    struct PARAMETER     *ptr_parameter;
    struct COMPOUNDSTMT  *ptr_compoundstmt;
    struct STMT          *ptr_stmt;
    struct ASSIGN        *ptr_assign;
    struct CALL          *ptr_call;
    struct ARG           *ptr_arg;
    struct WHILE_S       *ptr_while_s;
    struct FOR_S         *ptr_for_s;
    struct IF_S          *ptr_if_s;
    struct ID_S          *ptr_id_s;
    struct EXPR          *ptr_expr;
    Type_e type;
    //TODO int, float to char*
    int intnum;
    float floatnum;
    char* id;
}

%token <intnum>INTNUM <floatnum>FLOATNUM <id>ID
%token INT FLOAT MINUS PLUS MULT DIV LE GE EQ NE GT LT
%token IF ELSE FOR WHILE DO RETURN DQUOT_T SQUOT_T AMP_T 

%type <type> Type

%define parse.error verbose

%type <ptr_program> Program
%type <ptr_declaration> Declaration DeclList
%type <ptr_identifier> Identifier IdentList
%type <ptr_function> Function FuncList
%type <ptr_parameter> Parameter ParamList 
%type <ptr_compoundstmt> CompoundStmt
%type <ptr_stmt> Stmt StmtList
%type <ptr_assign> Assign AssignStmt 
%type <ptr_call> Call CallStmt
%type <ptr_arg> Arg ArgList
%type <ptr_while_s> While_s
%type <ptr_for_s> For_s
%type <ptr_if_s> If_s
%type <ptr_expr> Expr RetStmt
%type <ptr_id_s> Id_s;

%left LOWER_THAN_ELSE
%left ELSE
%left MULT DIV MINUS PLUS LE GE GT LT EQ NE

%start Program
%%
Program: DeclList FuncList {
            struct PROGRAM *prog = (struct PROGRAM*) malloc (sizeof (struct PROGRAM));
            prog->decl = $1;
            prog->func = $2;
            head = prog;
            $$ = prog;
       }
       | DeclList {
            struct PROGRAM *prog = (struct PROGRAM*) malloc (sizeof (struct PROGRAM));
            prog->decl = $1;
            prog->func = NULL;
            head = prog;
            $$ = prog;
       }
       | FuncList {
            struct PROGRAM *prog = (struct PROGRAM*) malloc (sizeof (struct PROGRAM));
            prog->decl = NULL;
            prog->func = $1;
            head = prog;
            $$ = prog;
       }
       ;
DeclList: Declaration {
            $$ = $1;
        }
        | DeclList Declaration {
            struct DECLARATION *decl;
            decl = $2;
            decl->prev = $1;
            $$ = decl;
        }
        ;
FuncList: Function {
            $$ = $1;
        }
        | FuncList Function {
            struct FUNCTION *func;
            func = $2;
            func->prev = $1;
            $$ = func;
        }
        ;
Declaration: Type IdentList ';' {
                struct DECLARATION *decl = (struct DECLARATION*) malloc (sizeof (struct DECLARATION));
                decl->t = $1;
                decl->id = $2;
                $$ = decl;
            }
           ;
IdentList: Identifier {
            $$ = $1;
        }
        | IdentList ',' Identifier {
            struct IDENTIFIER *iden;
            iden = $3;
            iden->prev = $1;
            $$ = iden;
        }
        ;
Identifier: ID {
            struct IDENTIFIER *iden = (struct IDENTIFIER*) malloc (sizeof (struct IDENTIFIER));
            iden->ID = $1;
            iden->intnum = 0;   // zero, If scalar
            iden->prev = NULL;
            $$ = iden;
          }
          | ID '[' INTNUM ']' {
            struct IDENTIFIER *iden = (struct IDENTIFIER*) malloc (sizeof (struct IDENTIFIER));
            iden->ID = $1;
            iden->intnum = $3;   // zero, If scalar
            iden->prev = NULL;
            $$ = iden;
           }
          ;
ParamList: Parameter {
            struct PARAMETER *param;
            param = $1;
            param->prev = NULL;
            $$ = param;
        }
         | ParamList ',' Parameter {
            struct PARAMETER *param;
            param = $3;
            param->prev = $1;
            $$ = param;
        }
Parameter: Type Identifier {
            struct PARAMETER *param = (struct PARAMETER*) malloc (sizeof (struct PARAMETER));
            param->t = $1;
            param->id = $2;
            param->prev = NULL;
            $$ = param;
        }
Function: Type ID '(' ')' CompoundStmt {
            struct FUNCTION *func = (struct FUNCTION*) malloc (sizeof (struct FUNCTION));
            func->t = $1;
            func->ID = $2;
            func->param = NULL;
            func->cstmt = $5;
            $$ = func;

        }
        | Type ID '(' ParamList ')' CompoundStmt {
        struct FUNCTION *func = (struct FUNCTION*) malloc (sizeof (struct FUNCTION));
        func->t = $1;
        func->ID = $2;
        func->param = $4;
        func->cstmt = $6;
        $$ = func;
    }
    ;
Type: INT { $$ = eInt;}
    | FLOAT { $$ = eFloat;}
    ;
//cf. Stmt 안에 CompoundStmt 존재
//StmtList 에서 empty 입력을 허용하지 않도록 StmtList 가 없는 Compound 정의
CompoundStmt: '{' '}' {
                struct COMPOUNDSTMT *comp = (struct COMPOUNDSTMT*) malloc (sizeof (struct COMPOUNDSTMT));
                comp->decl = NULL;
                comp->stmt = NULL;
                $$ = comp;
            }
            | '{' StmtList '}'  {
                struct COMPOUNDSTMT *comp = (struct COMPOUNDSTMT*) malloc (sizeof (struct COMPOUNDSTMT));
                comp->decl = NULL;
                comp->stmt = $2;
                $$ = comp;
            }
            |  '{' DeclList StmtList '}' {
                struct COMPOUNDSTMT *comp = (struct COMPOUNDSTMT*) malloc (sizeof (struct COMPOUNDSTMT));
                comp->decl = $2;
                comp->stmt = $3;
                $$ = comp;
            }
            |  '{' DeclList '}' {
                struct COMPOUNDSTMT *comp = (struct COMPOUNDSTMT*) malloc (sizeof (struct COMPOUNDSTMT));
                comp->decl = $2;
                comp->stmt = NULL;
                $$ = comp;
            }
            ;
StmtList: Stmt {
            struct STMT *stmt;
            stmt = $1;
            stmt->prev = NULL;
            $$ = stmt;
        }
        | StmtList Stmt {
            struct STMT *stmt;
            stmt = $2;
            stmt->prev = $1;
            $$ = stmt;
        }
        ;
Stmt: AssignStmt { 
        struct STMT *stmt = (struct STMT*) malloc (sizeof (struct STMT));
        stmt->s = eAssign;
        stmt->stmt.assign_ = $1;
        $$ = stmt;
    }
    | CallStmt {
        struct STMT *stmt = (struct STMT*) malloc (sizeof (struct STMT));
        stmt->s = eCall;
        stmt->stmt.call_ = $1;
        $$ = stmt;
    }
    | RetStmt {
        struct STMT *stmt = (struct STMT*) malloc (sizeof (struct STMT));
        stmt->s = eRet;
        stmt->stmt.return_ = $1;
        $$ = stmt;
    }
    | While_s {
        struct STMT *stmt = (struct STMT*) malloc (sizeof (struct STMT));
        stmt->s = eWhile;
        stmt->stmt.while_ = $1;
        $$ = stmt;
    }
    | For_s {
        struct STMT *stmt = (struct STMT*) malloc (sizeof (struct STMT));
        stmt->s = eFor;
        stmt->stmt.for_ = $1;
        $$ = stmt;
    }
    | If_s {
        struct STMT *stmt = (struct STMT*) malloc (sizeof (struct STMT));
        stmt->s = eIf;
        stmt->stmt.if_ = $1;
        $$ = stmt;
    }
    | CompoundStmt {
        struct STMT *stmt = (struct STMT*) malloc (sizeof (struct STMT));
        stmt->s = eCompound;
        stmt->stmt.cstmt_ = $1;
        $$ = stmt;
    }
    | ';' {
        struct STMT *stmt = (struct STMT*) malloc (sizeof (struct STMT));
        stmt->s = eSemi;
        $$ = stmt;
    }
    ;
AssignStmt: Assign ';' { 
            $$ = $1;
          }
          ;
Assign: ID '=' Expr {
            struct ASSIGN *assign = (struct ASSIGN*) malloc (sizeof (struct ASSIGN));
            assign->ID = $1;
            assign->index = NULL; //NUL, if LHS is scalar variable
            assign->expr = $3;
            $$ = assign;
        }
      | ID '[' Expr ']' '=' Expr {
            struct ASSIGN *assign = (struct ASSIGN*) malloc (sizeof (struct ASSIGN));
            assign->ID = $1;
            assign->index = $3; 
            assign->expr = $6;
            $$ = assign;
        }
      ;
CallStmt: Call ';' {
            $$ = $1;
        }
        ;
Call: ID '(' ')' {
        struct CALL *call = (struct CALL*) malloc (sizeof (struct CALL));
        call->ID = $1;
        call->arg = NULL;
        $$ = call;
    }
    | ID '(' ArgList ')' {
        struct CALL *call = (struct CALL*) malloc (sizeof (struct CALL));
        call->ID = $1;
        call->arg = $3;
        $$ = call;
    }
    ;
ArgList: Arg { $$ = $1;}
       | ArgList ',' Arg {
            struct ARG *arg;
            arg = $3;
            arg->prev = $1;
            $$ = arg;
        }
       ;
Arg: Expr {
    struct ARG *arg = (struct ARG*) malloc (sizeof (struct ARG));
    arg->expr = $1;
    arg->prev = NULL;
    $$ = arg;
   }
   ;
RetStmt: RETURN ';' {
        $$ = NULL;
        }
       | RETURN Expr ';' {
        $$ = $2;
       }
       ;
Expr: MINUS Expr{
        struct UNOP *unop = (struct UNOP*) malloc (sizeof (struct UNOP));
        unop->u = eNegative;
        unop->expr = $2;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eUnop;
        expr->expression.unop_ = unop;
        $$ = expr;
    }
    | Expr MINUS Expr{
        struct ADDIOP *addiop = (struct ADDIOP*) malloc (sizeof (struct ADDIOP));
		addiop->a = eMinus;
        addiop->lhs=$1;
        addiop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eAddi;
        expr->expression.addiop_ = addiop;
        $$ = expr;
    }
	| Expr PLUS Expr{
        struct ADDIOP *addiop = (struct ADDIOP*) malloc (sizeof (struct ADDIOP));
		addiop->a = ePlus;
        addiop->lhs=$1;
        addiop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eAddi;
        expr->expression.addiop_ = addiop;
        $$ = expr;
    }
    | Expr MULT Expr{
        struct MULTOP *multop = (struct MULTOP*) malloc (sizeof (struct MULTOP));
        multop->m = eMult;
        multop->lhs=$1;
        multop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eMulti;   // eMult와 다름 
        expr->expression.multop_ = multop;
        $$ = expr;
    }
	| Expr DIV Expr{
        struct MULTOP *multop = (struct MULTOP*) malloc (sizeof (struct MULTOP));
        multop->m = eDiv;
        multop->lhs=$1;
        multop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eMulti;   // eMult와 다름 
        expr->expression.multop_ = multop;
        $$ = expr;
    }
    | Expr LE Expr{
        struct RELAOP *relaop = (struct RELAOP*) malloc (sizeof (struct RELAOP));
        relaop->r = eLE;
        relaop->lhs=$1;
        relaop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eRela;  
        expr->expression.relaop_ = relaop;
        $$ = expr;
    }
	| Expr GE Expr{
        struct RELAOP *relaop = (struct RELAOP*) malloc (sizeof (struct RELAOP));
        relaop->r = eGE;
        relaop->lhs=$1;
        relaop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eRela;  
        expr->expression.relaop_ = relaop;
        $$ = expr;
    }
	| Expr GT Expr{
        struct RELAOP *relaop = (struct RELAOP*) malloc (sizeof (struct RELAOP));
        relaop->r = eGT;
        relaop->lhs=$1;
        relaop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eRela;  
        expr->expression.relaop_ = relaop;
        $$ = expr;
    }
	| Expr LT Expr{
        struct RELAOP *relaop = (struct RELAOP*) malloc (sizeof (struct RELAOP));
        relaop->r = eLT;
        relaop->lhs=$1;
        relaop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eRela;  
        expr->expression.relaop_ = relaop;
        $$ = expr;
    }
    | Expr EQ Expr {
        struct EQLTOP *eqltop = (struct EQLTOP*) malloc (sizeof (struct EQLTOP));
        eqltop->e = eEQ;
        eqltop->lhs=$1;
        eqltop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eEqlt;  
        expr->expression.eqltop_ = eqltop;
        $$ = expr;
    }
	| Expr NE Expr {
        struct EQLTOP *eqltop = (struct EQLTOP*) malloc (sizeof (struct EQLTOP));
        eqltop->e = eNE;
        eqltop->lhs=$1;
        eqltop->rhs=$3;

        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eEqlt;  
        expr->expression.eqltop_ = eqltop;
        $$ = expr;
    }
    | Call {
        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eCallExpr;  
        expr->expression.call_ = $1;
        $$ = expr;
    }
    | INTNUM {
        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eIntnum;  
        expr->expression.intnum = $1;
        $$ = expr;
    }    
    | FLOATNUM {
        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eFloatnum;  
        expr->expression.floatnum = $1;
        $$ = expr;
    }
    | Id_s {
        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eId;  
        expr->expression.ID_ = $1;
        $$ = expr;
    } 
    | '(' Expr ')' {
        struct EXPR *expr = (struct EXPR*) malloc (sizeof (struct EXPR));
        expr->e = eExpr;  
        expr->expression.bracket = $2;
        $$ = expr;
    }
    ;
Id_s: ID {
        struct ID_S *id_s = (struct ID_S*)malloc(sizeof (struct ID_S));
        id_s->ID = $1;
        id_s->expr = NULL;
        $$ = id_s;
    }
    | ID '[' Expr ']' {
        struct ID_S *id_s = (struct ID_S*)malloc(sizeof (struct ID_S));
        id_s->ID = $1;
        id_s->expr = $3;
        $$ = id_s;
    }
    ;
While_s: WHILE '(' Expr ')'  Stmt  {
           struct WHILE_S* while_s = (struct WHILE_S*) malloc (sizeof(struct WHILE_S));
           while_s->do_while = false;
           while_s->cond = $3;
           while_s->stmt = $5;
           $$ = while_s;
        }
         | DO  Stmt  WHILE '(' Expr ')' ';' {
           struct WHILE_S* while_s = (struct WHILE_S*) malloc (sizeof(struct WHILE_S));
           while_s->do_while = true;
           while_s->cond = $5;
           while_s->stmt = $2;
           $$ = while_s;
        }
         ;
For_s: FOR '(' Assign ';' Expr ';' Assign ')' Stmt {
           struct FOR_S *for_s = (struct FOR_S*) malloc (sizeof(struct FOR_S));
           for_s->init = $3;
           for_s->cond = $5;
           for_s->inc = $7;
           for_s->stmt = $9;
           $$ = for_s;
        }
       ;
If_s: IF '(' Expr ')' Stmt %prec LOWER_THAN_ELSE {
       struct IF_S *if_s = (struct IF_S*) malloc (sizeof(struct IF_S));
       if_s->cond=$3;
       if_s->if_=$5;
       if_s->else_=NULL;
       $$ = if_s;
    }
      | IF '(' Expr ')' Stmt ELSE Stmt{
       struct IF_S *if_s = (struct IF_S*) malloc (sizeof(struct IF_S));
       if_s->cond=$3;
       if_s->if_=$5;
       if_s->else_=$7;
       $$ = if_s;
      }
      ;
%%
void doProcess();

int check(int i, int opt, char* argv[]) {
    int valid = 1;
    if(strcmp(argv[i+1], "-h")) valid = 0;
    else if (strcmp(argv[i+1], "-int")) valid = 0;
    else if (strcmp(argv[i+1], "-integer")) valid = 0;
    else if (strcmp(argv[i+1], "-pdl")) valid = 0;
    else if (strcmp(argv[i+1], "-positive")) valid = 0;
    else if (strcmp(argv[i+1], "-smv")) valid = 0;
    else if (strcmp(argv[i+1], "-32b"))  valid = 0; 
    else if (strcmp(argv[i+1], "-64b")) valid = 0;
    if (opt == 1) {
        if (strcmp(argv[i+2], "-h")) valid = 0;
        else if (strcmp(argv[i+2], "-int")) valid = 0;
        else if (strcmp(argv[i+2], "-integer")) valid = 0;
        else if (strcmp(argv[i+2], "-pdl")) valid = 0;
        else if (strcmp(argv[i+2], "-positive")) valid = 0;
        else if (strcmp(argv[i+2], "-smv")) valid = 0;
        else if (strcmp(argv[i+2], "-32b")) valid = 0;
        else if (strcmp(argv[i+2], "-64b")) valid = 0;
    }
}

int parameters(int argc, char* argv[]){
    for(int i = 0; i< argc; i ++){
        if(!strcmp(argv[i], "-h")) {
            FILE *file=fopen("help.txt", "r");
            char ch;
            ch = fgetc(file);
            while (ch != EOF)
            {
                printf ("%c", ch);
                ch = fgetc(file);
            }
            fclose(file);
            return 1;
        }  else if (!strcmp(argv[i], "-int")) {
            option[0] = 1;
            pos[0] = i;
            if(argv[i+1] != NULL && argv[i+2] != NULL) {
                if (check(i, 1, argv)) {
                    int min = atoi(argv[i+1]);
                    int max = atoi(argv[i+2]);
                    char stri[12];
                    char stra[12];
                    sprintf(stri, "%d", min);
                    sprintf(stra, "%d", max);
                    if (!strcmp(stri, argv[i+1]) && !strcmp(stra, argv[i+2])){
                        range[0] = min;
                        range[1] = max;
                        return 0;
                    } else {
                        printf("Invalid Parameters");
                        return 1;
                    }
                } else {
                    printf("Invalid Parameters");
                    return 1;
                }
            } else {
                return 1;
            }
        }  else if (!strcmp(argv[i], "-integer")) {
            option[1] = 1;
            pos[1] = i;
            return 0;
        } else if(!strcmp(argv[i], "-pdl")) {
            option[2] = 1;
            if(argv[i+1] != NULL) {
                if (check(i, 0, argv)) {
                    int len = strlen(argv[i+1]);
                    if(!strncmp(argv[i+1] + len - 4,".txt",4)){
                    pdl = argv[i+1];
                    return 0;
                    } else {
                        printf("Invalid Parameters");
                        return 1;
                    }
                } else {
                    printf("Invalid Parameters");
                    return 1;
                }
            } else {
                printf("Invalid Parameters");
                return 1;
            }
        }  else if (!strcmp(argv[i], "-positive")) {
            option[3] = 1;
            return 0;
        } else if (!strcmp(argv[i], "-smv")) {
            option[4] = 1;
            pos [2] = i;
            if(argv[i+1] != NULL) {
                if (check(i, 0, argv)) {
                    int len = strlen(argv[i+1]);
                    if(!strncmp(argv[i+1] + len - 4,".smv",4)){
                        smv = argv[i+1];
                        return 0;
                    } else {
                        printf("Invalid Parameters");
                        return 1;
                    }
                } else {
                    printf("Invalid Parameters");
                    return 1;
                }
            } else {
                printf("Invalid Parameters");
                return 1;
            }
        } else if(!strcmp(argv[i], "-32b")) {
            option[5] = 1;
            return 0;
        } else if(!strcmp(argv[i], "-64b")) {
            option[6] = 1;
            return 0;
        } else {
            if(i == 1){
                if(!(yyin = fopen(argv[i], "r"))) {
                    fprintf(stderr, "File \"%s\" not found", &argv[i]);
                    return 2;
                }
            } else {
                if(pos[0]+1 != i && pos[0]+2 != i && pos[1] != i && pos[2] != i && i!=0 ) {
                    printf("Err: Invalid option: %s", argv[i]);
                    return 1;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    head = NULL;
    scopeHead = NULL;
    scopeTail = NULL;
    int result = parameters(argc, argv);
    if(result != 0){
        return result;
    }
    //print AST
    fp = fopen("tree.txt","w");
    fp2 = fopen("table.txt","w");
    if(!yyparse())
        doProcess();
    fprintf(fp, "\n");
    pclose(fp);
    pclose(fp2);
    return 0;
}
void doProcess() {
    //TODO
    if(head == NULL)
        exit(1);
    //make global node
    scopeHead = newScope(sGLOBAL, NULL);
    scopeTail = scopeHead;
    if(head->decl != NULL)
        visitDeclaration(head->decl);
    if(head->func != NULL)
        visitFunction(head->func);
	BuildTree(head, option, pdl, smv, range);
}
