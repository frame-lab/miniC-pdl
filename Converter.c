//Converte C para PDL utilizando MiniC
//autores: Allan Patrick e Philippe Geraldeli

#include <stdio.h>
#include <stdlib.h>
#include "print.h"
#include "symboltable.h"
#include "Converter.h"
#include "AST.h"
#include <string.h>
#include "PDLtoNux.h"

bool _isParam2 = false;
bool _needPrinted2 = false;
bool _isOtherComp2 = false;
bool _isCompound2 = false;
Type_e _curType2;
No *root;
Nolist *rootValor;
Nolist *auxValor;
No *aux;
No *aux2;
No *treeSet[100];
FILE *fp;
Numero *numero;
int setTam = -1;
int x = 0;
int y = 0;
int tag = 0;
int id_tag = 0;
int tam = 0;
int tamAux = 0;
int choice = 0;
int tamMethod = 0;
char *Parts3[100][4];
char *methodVector[100][2];

No *insert(No *no, int choices, char *chars)
{
    if (root == NULL)
    {
        root = (No *)malloc(sizeof(No));
        root->child = NULL;
        root->parent = NULL;
        root->chars = chars;
        aux = root;

        return root;
    }
    else
    {
        if (choices == 0)
        {
            No *noAux = (No *)malloc(sizeof(No));
            noAux->child = NULL;
            noAux->parent = NULL;
            noAux->chars = chars;
            no->child = noAux;
            aux = no->child;
            return aux;
        }
        else if (choices == 1)
        {
            choice = 0;
            No *noAux = (No *)malloc(sizeof(No));
            noAux->child = NULL;
            noAux->parent = NULL;
            noAux->chars = chars;
            treeSet[setTam]->parent = noAux;
            aux = treeSet[setTam]->parent;
            treeSet[setTam] = noAux;
            return aux;
        }
    }
}

void print(No *no)
{
    if (no != NULL)
    {

        printf("%s ", no->chars);

        fprintf(fp, "%s ", no->chars);

        if (no->child != NULL)
        {
            print(no->child);
        }
        if (no->parent != NULL)
        {
            print(no->parent);
        }
    }
}

void BuildTree(struct PROGRAM *head, int option[7], char *pdl, char *smv, int range[2])
{
    numero = (Numero *)malloc(sizeof(Numero));
    if (option[2] == 1)
    {
        fp = fopen(pdl, "w");
    }
    else
    {
        fp = fopen("resultado.txt", "w");
    }
    if (head->decl != NULL)
    {
        visitDeclaration2(head->decl);
    }
    if (head->func != NULL)
    {
        visitFunction2(head->func);
    }

    printf("\n");
    for (int i = 0; i < tamMethod; i++)
    {
        printf("%s = ", methodVector[i][0]);
        printf("%s\n", methodVector[i][1]);
    }
    printf("\n");
    for (int i = 0; i < tam; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (j == 1)
            {
                printf("= ");
            }
            printf("%s ", Parts3[i][j]);
        }
        printf("\n");
    }

    printf("\n");
    print(root);
    printf("\n");
    fclose(fp);

    conv(root, option, smv, range);
}

void visitDeclaration2(struct DECLARATION *decl)
{
    _isParam2 = false;
    if (decl->prev != NULL)
    {
        visitDeclaration2(decl->prev);
    }
    switch (decl->t)
    {
    case eInt:
        _curType2 = eInt;
        break;
    case eFloat:
        _curType2 = eFloat;
        break;
    default:
        exit(1);
    }
    _needPrinted2 = true;
    visitIdentifier2(decl->id);
    _needPrinted2 = false;
}

void visitFunction2(struct FUNCTION *func)
{
    if (func->prev != NULL)
    {
        visitFunction2(func->prev);
    }
    _curFuncName = func->ID;
    scopeTail = newScope(sFUNC, scopeTail);

    switch (func->t)
    {
    case eInt:
        break;
    case eFloat:
        break;
    default:
        exit(1);
    }
    if (strcmp(func->ID, "main"))
    {
        int present = 0;
        for (int i = 0; i < tamMethod; i++)
        {
            if (!strcmp(func->ID, methodVector[i][1]))
            {
                present = 1;
                insert(aux, choice, methodVector[i][0]);
            }
        }
        if (present == 0)
        {
            methodsFunction(func->ID);
        }
        TreeFunction();
        insert(aux, choice, "=");
    }
    insert(aux, choice, "(");
    if (!strcmp(func->ID, "main"))
    {
        TreeFunction();
    }

    if (func->param != NULL)
    {
        visitParameter2(func->param);
    }
    visitCompoundStmt2(func->cstmt);
    insert(aux, choice, ")\n");
    TreesubFunction();
    choice = 1;
    deleteScope(&scopeTail);
}
void visitIdentifier2(struct IDENTIFIER *iden)
{
    if (iden->prev != NULL)
    {
        visitIdentifier2(iden->prev);
    }
    if ((_curType2 == eInt) && (_isParam2 == false))
        insert(aux, choice, "int");
    if ((_curType2 == eFloat) && (_isParam2 == false))
        insert(aux, choice, "float");
    insert(aux, choice, iden->ID);
    insert(aux, choice, ";");
    choice = 1;
    if (iden->intnum > 0)
    {
        insert(aux, choice, "[");
        char num[255];

        snprintf(num, sizeof(iden->intnum), "%d", 10);

        insert(aux, choice, num);
        insert(aux, choice, "]");

        if (_needPrinted2 == true)
        {
            char *curType;
            if (_curType2 == eInt)
                curType = "int";
            else
                curType = "float";
        }
    }
    else if (iden->intnum < 0)
    {
        printf("minus array");
    }
    else
    {
        //scalar
        if (_needPrinted2 == true)
        {
            char *curType;
            if (_curType2 == eInt)
                curType = "int";
            else
                curType = "float";
        }
    }
}
void visitStmt2(struct STMT *stmt)
{
    if (stmt->prev != NULL)
    {
        visitStmt2(stmt->prev);
    }

    switch (stmt->s)
    {
    case eAssign:
        InsertSemicolon(stmt);
        visitAssignStmt2(stmt->stmt.assign_);
        break;

    case eCall:
        InsertSemicolon(stmt);
        visitCallStmt2(stmt->stmt.call_);
        break;

    case eRet:
        InsertSemicolon(stmt);
        if (stmt->stmt.return_ == NULL)
        {
            insert(aux, choice, "return");
            insert(aux, choice, ";");
            TreeFunction();
        }
        else
        {
            aux2 = aux;
            insert(aux, choice, "return");
            TreeFunction();
            visitExpr2(stmt->stmt.return_);
        }
        break;

    case eWhile:
        InsertSemicolon(stmt);
        _isOtherComp2 = true;
        visitWhile_s2(stmt->stmt.while_);
        return;
    case eFor:
        InsertSemicolon(stmt);
        _isOtherComp2 = true;
        visitFor_s2(stmt->stmt.for_);
        return;
    case eIf:
        InsertSemicolon(stmt);
        _isOtherComp2 = true;
        visitIf_s2(stmt->stmt.if_);
        return;

    case eCompound:
        InsertSemicolon(stmt);
        if (_isOtherComp2 == false)
            _isCompound2 = true;
        visitCompoundStmt2(stmt->stmt.cstmt_);
        return;

    case eSemi:
        InsertSemicolon(stmt);
        break;
    }
}
void visitParameter2(struct PARAMETER *param)
{
    _isParam2 = true;
    if (param->prev != NULL)
    {
        visitParameter2(param->prev);
        if (strcmp(aux->chars, ";"))
        {
            insert(aux, choice, ",");
        }
    }
    switch (param->t)
    {
    case eInt:
        insert(aux, choice, "int");
        _curType2 = eInt;
        break;
    case eFloat:
        insert(aux, choice, "float");
        _curType2 = eFloat;
        break;
    default:
        printf("Declaration does not exist.\n");
        exit(1);
    }
    _needPrinted2 = true;
    visitIdentifier2(param->id);
    _needPrinted2 = false;
}
void visitCompoundStmt2(struct COMPOUNDSTMT *cstmt)
{
    if (_isCompound2 == true)
    {
        scopeTail = newScope(sCOMPOUND, scopeTail);
        scopeTail->parent->compound_n++;
    }
    _isOtherComp2 = false;
    if (cstmt->decl != NULL)
    {
        visitDeclaration2(cstmt->decl);
    }
    if (cstmt->stmt != NULL)
        visitStmt2(cstmt->stmt);

    if (_isCompound2 == true)
    {
        deleteScope(&scopeTail);
    }
    _isCompound2 = false;
    _isOtherComp2 = false;
}
void visitAssignStmt2(struct ASSIGN *assign)
{
    aux2 = (No *)malloc(sizeof(No));
    aux2->child = NULL;
    aux2->parent = NULL;
    aux2->chars = assign->ID;
    if (assign->index != NULL)
    {
        insert(aux, choice, "[");
        visitExpr2(assign->index);
        insert(aux, choice, "]");
    }
    switch (assign->expr->e)
    {
    case eRela:
        insert(aux, choice, assign->ID);
        TreeFunction();
        break;
    case eIntnum:
        insert(aux, choice, assign->ID);
        TreeFunction();
        insert(aux, choice, "=");
        break;
    case eFloatnum:
        insert(aux, choice, assign->ID);
        TreeFunction();
        insert(aux, choice, "=");
        break;
    case eId:
        insert(aux, choice, assign->ID);
        TreeFunction();
        insert(aux, choice, "=");
        break;
    }

    visitExpr2(assign->expr);
}
void visitCallStmt2(struct CALL *call)
{
    int present = 0;
    for (int i = 0; i < tamMethod; i++)
    {
        if (!strcmp(call->ID, methodVector[i][1]))
        {
            present = 1;
            insert(aux, choice, methodVector[i][0]);
        }
    }
    if (present == 0)
    {
        methodsFunction(call->ID);
    }
    TreeFunction();
    insert(aux, choice, "(");
    if (call->arg != NULL)
    {
        visitArg2(call->arg);
    }
    insert(aux, choice, ")");
}
void visitArg2(struct ARG *arg)
{
    if (arg->prev != NULL)
    {
        visitArg2(arg->prev);
        insert(aux, choice, ",");
    }
    aux2 = aux;
    visitExpr2(arg->expr);
}

void visitExpr2(struct EXPR *expr)
{
    switch (expr->e)
    {
    case eUnop:
        insert(aux, choice, "-");
        visitExpr2(expr->expression.unop_->expr);
        break;

    case eAddi:
        id_tag = 1;
        tamAux = tam;
        tag++;
        visitExpr2(expr->expression.addiop_->lhs);
        tag--;
        if (expr->expression.addiop_->a == ePlus)
        {
            Parts3[tag + tamAux][2] = "+";
        }
        else
        {
            Parts3[tag + tamAux][2] = "-";
        }

        visitExpr2(expr->expression.addiop_->rhs);

        if (tag != 0)
            Parts(expr->expression.addiop_->rhs);
        else
        {
            Parts3[tam][0] = aux2->chars;
            tam++;
            for (int i = tamAux; i < tam; i++)
            {
                if (i > tamAux)
                {
                    Parts3[i][3] = Parts3[i - 1][0];
                }
            }
            for (tamAux; tamAux < tam; tamAux++)
            {
                insert(aux, choice, Parts3[tamAux][0]);
                insert(aux, choice, "=");
                insert(aux, choice, Parts3[tamAux][1]);
                insert(aux, choice, Parts3[tamAux][2]);
                insert(aux, choice, Parts3[tamAux][3]);
                if (!(tamAux + 1 == tam))
                    insert(aux, choice, ";");
            }
            id_tag = 0;
        }

        break;

    case eMulti:
        id_tag = 1;
        tamAux = tam;
        tag++;
        visitExpr2(expr->expression.multop_->lhs);
        tag--;
        if (expr->expression.multop_->m == eMult)
        {
            Parts3[tag + tamAux][2] = "*";
        }
        else
        {
            Parts3[tag + tamAux][2] = "/";
        }

        visitExpr2(expr->expression.multop_->rhs);

        if (tag != 0)
            Parts(expr->expression.multop_->rhs);
        else
        {
            Parts3[tam][0] = aux2->chars;
            tam++;
            for (int i = tamAux; i < tam; i++)
            {
                if (i > tamAux)
                {
                    Parts3[i][3] = Parts3[i - 1][0];
                }
            }
            for (tamAux; tamAux < tam; tamAux++)
            {
                insert(aux, choice, Parts3[tamAux][0]);
                insert(aux, choice, "=");
                insert(aux, choice, Parts3[tamAux][1]);
                insert(aux, choice, Parts3[tamAux][2]);
                insert(aux, choice, Parts3[tamAux][3]);
                if (!(tamAux + 1 == tam))
                    insert(aux, choice, ";");
            }
            id_tag = 0;
        }

        break;

    case eRela:
        visitExpr2(expr->expression.relaop_->lhs);
        switch (expr->expression.relaop_->r)
        {
        case eLT:
            insert(aux, choice, "<");
            break;

        case eGT:
            insert(aux, choice, ">");
            break;

        case eLE:
            insert(aux, choice, "<=");
            break;

        case eGE:
            insert(aux, choice, ">=");
            break;
        }
        visitExpr2(expr->expression.relaop_->rhs);
        break;

    case eEqlt:
        visitExpr2(expr->expression.eqltop_->lhs);
        if (expr->expression.eqltop_->e == eEQ)
        {
            insert(aux, choice, "==");
        }
        else
        {
            insert(aux, choice, "!=");
        }
        visitExpr2(expr->expression.eqltop_->rhs);
        break;

    case eCallExpr:
        visitCallStmt2(expr->expression.call_);
        break;

    case eExpr:
        insert(aux, choice, "(");
        visitExpr2(expr->expression.bracket);
        insert(aux, choice, ")");
        break;

    case eId:
        visitId_s2(expr->expression.ID_);
        break;

    case eIntnum:
        switch (id_tag)
        {
        case 0:
            if (numero != NULL)
            {
                snprintf(numero->num, 255 * sizeof(char), "%d", (int)expr->expression.intnum);
                insert(aux, choice, numero->num);
                numero = numero->prox;
            }
            else if (numero == NULL)
            {
                Numero *novo = (Numero *)malloc(sizeof(Numero));
                numero = novo;
                snprintf(numero->num, 255 * sizeof(char), "%d", (int)expr->expression.intnum);
                insert(aux, choice, numero->num);
                numero = numero->prox;
            }
            break;
        case 1:
            if (numero != NULL)
            {
                snprintf(numero->num, 255 * sizeof(char), "%d", (int)expr->expression.intnum);
                if (tag == 0)
                {
                    Parts3[tamAux][3] = numero->num;
                }
                else if (tag == 1)
                {
                    Parts3[tamAux][1] = numero->num;
                }
                else
                {
                    Parts3[tag - 1][1] = numero->num;
                }
                numero = numero->prox;
            }
            else if (numero == NULL)
            {
                Numero *novo = (Numero *)malloc(sizeof(Numero));
                numero = novo;
                snprintf(numero->num, 255 * sizeof(char), "%d", (int)expr->expression.intnum);
                if (tag == 0)
                {
                    Parts3[tamAux][3] = numero->num;
                }
                else if (tag == 1)
                {
                    Parts3[tamAux][1] = numero->num;
                }
                else
                {
                    Parts3[tag - 1][1] = numero->num;
                }
                numero = numero->prox;
            }
            break;
        }
        break;

    case eFloatnum:
        switch (id_tag)
        {
        case 0:
            if (numero != NULL)
            {
                snprintf(numero->num, 255 * sizeof(char), "%f", (float)expr->expression.floatnum);
                insert(aux, choice, numero->num);
                numero = numero->prox;
            }
            else if (numero == NULL)
            {
                Numero *novo = (Numero *)malloc(sizeof(Numero));
                numero = novo;
                snprintf(numero->num, 255 * sizeof(char), "%f", (float)expr->expression.floatnum);
                insert(aux, choice, numero->num);
                numero = numero->prox;
            }
            break;
        case 1:
            if (numero != NULL)
            {
                snprintf(numero->num, 255 * sizeof(char), "%f", (float)expr->expression.floatnum);
                if (tag == 0)
                {
                    Parts3[tamAux][3] = numero->num;
                }
                else if (tag == 1)
                {
                    Parts3[tamAux][1] = numero->num;
                }
                else
                {
                    Parts3[tag - 1][1] = numero->num;
                }
                numero = numero->prox;
            }
            else if (numero == NULL)
            {
                Numero *novo = (Numero *)malloc(sizeof(Numero));
                numero = novo;
                snprintf(numero->num, 255 * sizeof(char), "%f", (float)expr->expression.floatnum);
                if (tag == 0)
                {
                    Parts3[tamAux][3] = numero->num;
                }
                else if (tag == 1)
                {
                    Parts3[tamAux][1] = numero->num;
                }
                else
                {
                    Parts3[tag - 1][1] = numero->num;
                }
                numero = numero->prox;
            }
            break;
        }
        break;
    }
}

void visitWhile_s2(struct WHILE_S *while_s)
{
    if (while_s->do_while == true)
    {
        scopeTail = newScope(sDOWHILE, scopeTail);
        scopeTail->parent->dowhile_n++;
        insert(aux, choice, "(");
        TreeFunction();
        visitStmt2(while_s->stmt);
        if (while_s->stmt != NULL)
        {
            insert(aux, choice, ";");
        }
        choice = 1;
        insert(aux, choice, "(");
        TreeFunction();
        aux2 = aux;
        visitExpr2(while_s->cond);
        insert(aux, choice, ")");
        insert(aux, choice, "?");
        choice = 1;
        insert(aux, choice, ")");
        insert(aux, choice, "*");
        insert(aux, choice, ";");
        choice = 1;
        insert(aux, choice, "¬");
        insert(aux, choice, "(");
        aux2 = aux;
        visitExpr2(while_s->cond);
        insert(aux, choice, ")");
        insert(aux, choice, "?");
        insertList(while_s->cond);
    }
    else
    {
        scopeTail = newScope(sWHILE, scopeTail);
        scopeTail->parent->while_n++;
        insert(aux, choice, "(");
        insert(aux, choice, "(");
        TreeFunction();
        aux2 = aux;
        visitExpr2(while_s->cond);
        insert(aux, choice, ")");
        insert(aux, choice, "?");
        insert(aux, choice, ";");
        choice = 1;
        visitStmt2(while_s->stmt);
        insert(aux, choice, ")");
        insert(aux, choice, "*");
        insert(aux, choice, ";");
        choice = 1;
        insert(aux, choice, "¬");
        insert(aux, choice, "(");
        TreeFunction();
        aux2 = aux;
        visitExpr2(while_s->cond);
        insert(aux, choice, ")");
        insert(aux, choice, "?");
        insertList(while_s->cond);
    }
    deleteScope(&scopeTail);
}
void visitFor_s2(struct FOR_S *for_s)
{
    scopeTail = newScope(sFOR, scopeTail);
    scopeTail->parent->for_n++;
    insert(aux, choice, "(");
    TreeFunction();
    visitAssignStmt2(for_s->init);
    insert(aux, choice, ";");
    choice = 1;
    insert(aux, choice, "(");
    TreeFunction();
    aux2 = aux;
    visitExpr2(for_s->cond);
    insert(aux, choice, ")");
    insert(aux, choice, "?");
    insert(aux, choice, ";");
    choice = 1;
    visitStmt2(for_s->stmt);
    insert(aux, choice, ";");
    choice = 1;
    visitAssignStmt2(for_s->inc);
    insert(aux, choice, ")");
    insert(aux, choice, "*");
    insert(aux, choice, ";");
    choice = 1;
    insert(aux, choice, "¬");
    insert(aux, choice, "(");
    TreeFunction();
    aux2 = aux;
    visitExpr2(for_s->cond);
    insert(aux, choice, ")");
    insert(aux, choice, "?");
    insertList(for_s->cond);
    deleteScope(&scopeTail);
}
void visitIf_s2(struct IF_S *if_s)
{
    scopeTail = newScope(sIF, scopeTail);
    scopeTail->parent->if_n++;

    insert(aux, choice, "(");
    insert(aux, choice, "(");
    TreeFunction();
    aux2 = aux;
    visitExpr2(if_s->cond);
    insert(aux, choice, ")");
    insert(aux, choice, "?");
    insertList(if_s->cond);
    if (if_s->if_->s)
    {
        insert(aux, choice, ";");
    }
    choice = 1;
    visitStmt2(if_s->if_);
    insert(aux, choice, ")");
    choice = 1;
    if (if_s->else_ != NULL)
    {
        insert(aux, choice, "U");
        insert(aux, choice, "(");
        insert(aux, choice, "¬");
        insert(aux, choice, "(");
        TreeFunction();
        aux2 = aux;
        visitExpr2(if_s->cond);
        insert(aux, choice, ")");
        insert(aux, choice, "?");
        if (if_s->else_->s)
        {
            insert(aux, choice, ";");
        }
        choice = 1;
        visitStmt2(if_s->else_);
        insert(aux, choice, ")");
    }
    else
    {
        insert(aux, choice, "U");
        choice = 1;
        insert(aux, choice, "¬");
        insert(aux, choice, "(");
        TreeFunction();
        aux2 = aux;
        visitExpr2(if_s->cond);
        insert(aux, choice, ")");
        insert(aux, choice, "?");
    }
}
void visitId_s2(struct ID_S *id_s)
{
    if (id_tag == 0)
    {
        insert(aux, choice, id_s->ID);
    }
    else
    {
        if (tag == 0)
        {
            Parts3[tamAux][3] = id_s->ID;
        }
        else if (tag == 1)
        {
            Parts3[tamAux][1] = id_s->ID;
        }
        else
        {
            Parts3[tag - 1][1] = id_s->ID;
        }
    }

    if (id_s->expr != NULL)
    {
        insert(aux, choice, "[");
        aux2 = aux;
        visitExpr2(id_s->expr);
        insert(aux, choice, "]");
    }
}

void InsertSemicolon(struct STMT *stmt)
{
    if (stmt->prev != NULL)
    {
        insert(aux, choice, ";");
        choice = 1;
    }
}

void Parts(struct EXPR *expr)
{
    char var[] = "_tX";
    int buffersize = 100;
    char *variable = malloc(buffersize);
    switch (expr->e)
    {
    case eId:
        var[2] = ++x + '0';
        strncpy(variable, var, buffersize);
        Parts3[tam][0] = variable;
        tam++;
        break;
    case eIntnum:
        var[2] = ++x + '0';
        strncpy(variable, var, buffersize);
        Parts3[tam][0] = variable;
        tam++;
        break;
    case eFloatnum:
        var[2] = ++x + '0';
        strncpy(variable, var, buffersize);
        Parts3[tam][0] = variable;
        tam++;
        break;
    default:
        break;
    }
}

void methodsFunction(char *ID)
{
    char var[] = "_yX";
    int buffersize = 100;
    char *variable = malloc(buffersize);
    var[2] = ++y + '0';
    strncpy(variable, var, buffersize);
    methodVector[tamMethod][0] = variable;
    methodVector[tamMethod][1] = ID;
    insert(aux, choice, methodVector[tamMethod][0]);
    tamMethod++;
}

void TreeFunction()
{
    setTam++;
    treeSet[setTam] = aux;
}

void TreesubFunction()
{
    treeSet[setTam] = NULL;
    setTam--;
}

void insertList(struct EXPR *expr)
{
    /*
        Logica para acessar a arvore e pegar o valor e a condição de todas as condições utilizadas no programa
        com o proposito de adicionar em uma lista que será usada para descrever as transições de formulas no NuXmV;
    */

    char *lhs = NULL;
    char *rhs = NULL;
    char *relation;

    switch (expr->e)
    {
    case eRela:
        lhs = visitExprValue(lhs, expr->expression.relaop_->lhs);
        switch (expr->expression.relaop_->r)
        {
        case eLT:
            relation = "<";
            break;
        case eGT:
            relation = ">";
            break;
        case eLE:
            relation = "<=";
            break;
        case eGE:
            relation = ">=";
            break;
        }
        rhs = visitExprValue(rhs, expr->expression.relaop_->rhs);
        break;
    case eEqlt:
        lhs = visitExprValue(lhs, expr->expression.eqltop_->lhs);
        switch (expr->expression.eqltop_->e)
        {
        case eEQ:
            relation = "==";
            break;
        case eNE:
            relation = "!=";
            break;
        }
        rhs = visitExprValue(rhs, expr->expression.eqltop_->rhs);
        break;
    }

    if (rootValor == NULL)
    {
        rootValor = (Nolist *)malloc(sizeof(Nolist));
        Nolist *NoauxValor = (Nolist *)malloc(sizeof(Nolist));
        Nolist *NoauxValor2 = (Nolist *)malloc(sizeof(Nolist));

        rootValor->chars = lhs;
        rootValor->next = NoauxValor;

        NoauxValor->chars = relation;
        NoauxValor->next = NoauxValor2;

        NoauxValor2->chars = rhs;
        NoauxValor2->next = NULL;

        auxValor = NoauxValor2;
    }
    else
    {
        Nolist *NoauxValor = (Nolist *)malloc(sizeof(Nolist));
        Nolist *NoauxValor2 = (Nolist *)malloc(sizeof(Nolist));
        Nolist *NoauxValor3 = (Nolist *)malloc(sizeof(Nolist));

        auxValor->next = NoauxValor;

        NoauxValor->chars = lhs;
        NoauxValor->next = NoauxValor2;

        NoauxValor2->chars = relation;
        NoauxValor2->next = NoauxValor3;

        NoauxValor3->chars = rhs;
        NoauxValor3->next = NULL;

        auxValor = NoauxValor3;
    }
}

char *visitExprValue(char *current, struct EXPR *expr)
{
    char *aux;

    if (current == NULL)
    {
        switch (expr->e)
        {
        case eId:
            return expr->expression.ID_->ID;
            break;
        case eIntnum:
            if (numero != NULL)
            {
                snprintf(numero->num, 255 * sizeof(char), "%d", (int)expr->expression.intnum);
                current = numero->num;
                numero = numero->prox;
                return current;
            }
            else
            {
                Numero *novo = (Numero *)malloc(sizeof(Numero));
                numero = novo;
                snprintf(numero->num, 255 * sizeof(char), "%d", (int)expr->expression.intnum);
                current = numero->num;
                numero = numero->prox;
                return current;
            }
            break;
        case eFloatnum:
            if (numero != NULL)
            {
                snprintf(numero->num, 255 * sizeof(char), "%f", (float)expr->expression.floatnum);
                current = numero->num;
                numero = numero->prox;
            }
            else
            {
                Numero *novo = (Numero *)malloc(sizeof(Numero));
                numero = novo;
                snprintf(numero->num, 255 * sizeof(char), "%f", (float)expr->expression.floatnum);
                current = numero->num;
                numero = numero->prox;
            }
            return current;
            break;
        case eUnop:
            current = "-";
            visitExprValue(current, expr->expression.unop_->expr);
            break;
        case eAddi:
            current = visitExprValue(current, expr->expression.addiop_->lhs);
            if (expr->expression.addiop_->a == ePlus)
            {
                aux = "+";
            }
            else
            {
                aux = "-";
            }
            current = concat_strings(current, aux);
            return visitExprValue(current, expr->expression.addiop_->rhs);
            break;
        case eMulti:
            current = visitExprValue(current, expr->expression.multop_->lhs);
            if (expr->expression.multop_->m == eMult)
            {
                aux = "*";
            }
            else
            {
                aux = "/";
            }
            current = concat_strings(current, aux);
            return visitExprValue(current, expr->expression.multop_->rhs);
            break;
        }
    }
    else
    {
        switch (expr->e)
        {
        case eId:
            return concat_strings(current, expr->expression.ID_->ID);
            break;
        case eIntnum:
            if (numero != NULL)
            {
                snprintf(numero->num, 255 * sizeof(char), "%d", (int)expr->expression.intnum);
                aux = numero->num;
                numero = numero->prox;
                return concat_strings(current, aux);
            }
            else
            {
                Numero *novo = (Numero *)malloc(sizeof(Numero));
                numero = novo;
                snprintf(numero->num, 255 * sizeof(char), "%d", (int)expr->expression.intnum);
                aux = numero->num;
                numero = numero->prox;
                return concat_strings(current, aux);
            }
            break;
        case eFloatnum:
            if (numero != NULL)
            {
                snprintf(numero->num, 255 * sizeof(char), "%f", (float)expr->expression.floatnum);
                aux = numero->num;
                numero = numero->prox;
            }
            else
            {
                Numero *novo = (Numero *)malloc(sizeof(Numero));
                numero = novo;
                snprintf(numero->num, 255 * sizeof(char), "%f", (float)expr->expression.floatnum);
                aux = numero->num;
                numero = numero->prox;
            }
            return concat_strings(current, aux);
            break;
        case eUnop:
            current = "-";
            visitExprValue(current, expr->expression.unop_->expr);
            break;
        case eAddi:
            current = concat_strings(current, visitExprValue(current, expr->expression.addiop_->lhs));
            if (expr->expression.addiop_->a == ePlus)
            {
                aux = "+";
            }
            else
            {
                aux = "-";
            }
            current = concat_strings(current, aux);
            return visitExprValue(current, expr->expression.addiop_->rhs);
            break;
        case eMulti:
            current = concat_strings(current, visitExprValue(current, expr->expression.multop_->lhs));
            if (expr->expression.multop_->m == eMult)
            {
                aux = "*";
            }
            else
            {
                aux = "/";
            }
            current = concat_strings(current, aux);
            return visitExprValue(current, expr->expression.multop_->rhs);
            break;
        }
    }
}

char *concat_strings(char *str1, char *str2)
{
    char *str3 = (char *)malloc(1 + strlen(str1) + strlen(str2));
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}