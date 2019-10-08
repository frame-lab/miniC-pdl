#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PDLtoNux.h"

int *opt;
int *ran;
int end = 0;
int first = 0;
int warning = 0;
int treeTam = 0;
int notTam = 0;
int isNotDet = 0;
int isNotEnd = 0;
int isPar = 0;
int isNotDetSecond = 0;
int isStar = 0;
int condPos = 0;
int bracketPos = -1;
int posCondChar = 0;
int posCondNumber = 0;
int bracket[100];
int cond[100];
int notDeterministic[100][100];
int detEnd[100][100];
int condCharPos[100][100];
char *treeElements[100];
char *condChar[100][2];
char *condNumber[100][100];
No *newFunction;
FILE *fp;

void conv(No *no, int option[7], char *smv, int range[2])
{
    if (option[4] == 1)
    {
        fp = fopen(smv, "w");
    }
    else
    {
        fp = fopen("nuX.smv", "w");
    }

    opt = option;
    ran = range;
    if (no != NULL)
    {
        mod(no);
    }
    if (warning == 1)
    {
        printf("\n\nWarning: unused variables were set to 0\n\n");
    }
}

void mod(No *no)
{
    newFunction = NULL;
    if (!strcmp(no->chars, "("))
    {
        fprintf(fp, "MODULE main\n");

        printf("MODULE main\n");
        var(no);
    }
    else
    {
        fprintf(fp, "MODULE %s\n", no->chars);

        printf("MODULE %s\n", no->chars);
        first = 1;
        var(no);
    }
}

void var(No *no)
{
    fprintf(fp, "VAR\n\tprog : {");

    printf("VAR\n\tprog : {");
    newFunction = prog(no);
    fprintf(fp, "};");

    printf("};");
    for (int i = 0; i < treeTam; i++)
    {
        val(i);
    }
    for (int i = treeTam - 1; i >= 0; i--)
    {
        addchild(i);
    }
    Assign();
    for (int i = 0; i < posCondChar; i++)
    {
        valAssign(i);
    }
    next();
    for (int i = 0; i < posCondChar; i++)
    {
        valNext(i);
    }
    if (newFunction != NULL)
    {
        end = 0;
        first = 0;
        warning = 0;
        treeTam = 0;
        notTam = 0;
        isNotDet = 0;
        isNotEnd = 0;
        isPar = 0;
        isNotDetSecond = 0;
        isStar = 0;
        condPos = 0;
        bracketPos = -1;
        posCondChar = 0;
        posCondNumber = 0;
        memset(bracket, 0, sizeof(bracket));
        memset(cond, 0, sizeof(cond));
        memset(notDeterministic, 0, sizeof(notDeterministic));
        memset(detEnd, 0, sizeof(detEnd));
        memset(condCharPos, 0, sizeof(condCharPos));
        memset(treeElements, 0, sizeof(treeElements));
        memset(condChar, 0, sizeof(condChar));
        memset(condNumber, 0, sizeof(condNumber));
        printf("\n\n");
        mod(newFunction);
    }
}

No *prog(No *no)
{
    if (no != NULL)
    {
        if (first == 1 && strcmp(no->chars, "("))
        {
            first = 0;
            prog(no->child->child);
            printf(", ");
            prog(no->parent);
        }
        else
        {
            if (!strcmp(no->chars, ")"))
            {
                if (isNotEnd == 1)
                {
                    isPar = 1;
                }
            }
            else if (!strcmp(no->chars, "U"))
            {
                isNotDet = 1;
                isNotEnd = 1;
                int m = 0;
                while (notDeterministic[bracket[bracketPos]][m])
                {
                    m++;
                }
                notDeterministic[bracket[bracketPos]][m] = treeTam;
                detEnd[treeTam - 1][0] = -1;
                bracketPos--;
            }
            else if (!strcmp(no->chars, "?"))
            {
                cond[condPos] = treeTam;
                condPos++;
            }
            else if (!strcmp(no->chars, "("))
            {
                if (bracketPos > -1 && isNotDet == 0 && !isStar)
                {
                    bracket[bracketPos] = treeTam;
                    if (!isNotDetSecond)
                    {
                        bracketPos++;
                    }
                    else
                    {
                        isNotDetSecond = 0;
                    }
                }
                else if (isNotDet == 1)
                {
                    isNotDet = 0;
                    int m = 0;
                    while (notDeterministic[bracket[bracketPos - 1]][m])
                    {
                        m++;
                    }
                    notDeterministic[bracket[bracketPos - 1]][m] = treeTam;
                    bracketPos++;
                    isNotDetSecond = 1;
                }
                else
                {
                    bracketPos++;
                    isStar = 0;
                }
            }
            else if (!strcmp(no->chars, "*"))
            {
                notDeterministic[treeTam + 1][0] = bracket[bracketPos - 1];
                notDeterministic[treeTam + 1][1] = treeTam + 1;
                int m = 0;
                while (notDeterministic[bracketPos - 1][m])
                {
                    m++;
                }
                notDeterministic[bracket[bracketPos - 1]][m] = bracket[bracketPos - 1];
                notDeterministic[bracket[bracketPos - 1]][m] = treeTam + 1;
                isStar = 1;
            }
            if (strcmp(no->chars, "U") && strcmp(no->chars, ";") && strcmp(no->chars, "(") && strcmp(no->chars, ")") && strcmp(no->chars, ")\n") && strcmp(no->chars, "_") && strcmp(no->chars, "*"))
            {
                treeElements[treeTam] = stringMemory(treeElements[treeTam], no->chars);
                if (!strcmp(no->chars, "¬"))
                {
                    treeElements[treeTam] = stringMemory(treeElements[treeTam], "_");
                }
            }

            if (no->child != NULL)
            {
                if (strcmp(no->child->chars, ";") && strcmp(no->chars, "(") && strcmp(no->child->chars, "(") && strcmp(no->child->chars, ")") && strcmp(no->child->chars, ")\n") && strcmp(no->child->chars, "*"))
                {
                    treeElements[treeTam] = stringMemory(treeElements[treeTam], "_");
                }
                if (!strcmp(no->child->chars, ")\n"))
                {
                    end = 1;
                }
                prog(no->child);
            }
            else
            {
                if (treeElements[treeTam] != NULL)
                {
                    fprintf(fp, "%s_%d", treeElements[treeTam], treeTam);

                    printf("%s_%d", treeElements[treeTam], treeTam);
                    treeTam++;
                }
                else
                {
                    treeElements[treeTam] = NULL;
                }
            }
            if (no->parent != NULL)
            {
                if (end == 1)
                {
                    end = 0;
                    return no->parent;
                }
                else
                {
                    fprintf(fp, ", ");

                    printf(", ");
                    if (strcmp(no->parent->chars, "U") && isPar == 1 && isNotEnd == 1)
                    {
                        isPar = 0;
                        isNotEnd = 0;
                        for (int m = 0; m < treeTam; m++)
                        {
                            if (detEnd[m][0] == -1)
                            {
                                detEnd[m][0] = treeTam;
                            }
                        }
                    }
                    prog(no->parent);
                }
            }
        }
    }
}

void Assign()
{
    if (0 == notDeterministic[0][0])
    {
        fprintf(fp, "\nASSIGN\n\tinit(prog) := {");

        printf("\nASSIGN\n\tinit(prog) := {");
        int m = 0;
        fprintf(fp, "%s_%d", treeElements[0], 0);

        printf("%s_%d", treeElements[0], 0);
        while (notDeterministic[0][m])
        {
            fprintf(fp, ", %s_%d", treeElements[notDeterministic[0][m]], notDeterministic[0][m]);

            printf(", %s_%d", treeElements[notDeterministic[0][m]], notDeterministic[0][m]);
            m++;
        }
        fprintf(fp, "};");

        printf("};");
    }
    else
    {
        fprintf(fp, "\nASSIGN\n\tinit(prog) := ");

        printf("\nASSIGN\n\tinit(prog) := ");
        fprintf(fp, "%s_%d;", treeElements[0], 0);

        printf("%s_%d;", treeElements[0], 0);
    }
}

void next()
{
    fprintf(fp, "\n\tnext(prog) := case");

    printf("\n\tnext(prog) := case");
    elements();

    fprintf(fp, "\n\t\t\t\tesac;");

    printf("\n\t\tesac;");
}

void elements()
{
    for (int i = 0; i < treeTam; i++)
    {

        if (treeElements[i + 1] != NULL)
        {
            int isPresent = 0;
            fprintf(fp, "\n\t\t\t\t\tprog = %s_%d: ", treeElements[i], i);

            printf("\n\t\tprog = %s_%d: ", treeElements[i], i);
            int x = 0;
            if (detEnd[i][0] != 0)
            {
                if (notDeterministic[detEnd[i][0] + 1][x])
                {
                    fprintf(fp, "{");

                    printf("{");
                    fprintf(fp, "%s_%d", treeElements[detEnd[i][0] + 1], i + 1);

                    printf("%s_%d", treeElements[detEnd[i][0] + 1], i + 1);
                    while (notDeterministic[detEnd[i][0] + 1][x])
                    {
                        fprintf(fp, ",");

                        printf(",");
                        fprintf(fp, " %s_%d", treeElements[notDeterministic[detEnd[i][0] + 1][x]], notDeterministic[detEnd[i][0] + 1][x]);

                        printf(" %s_%d", treeElements[notDeterministic[detEnd[i][0] + 1][x]], notDeterministic[detEnd[i][0] + 1][x]);
                        x++;
                    }
                    fprintf(fp, "};");

                    printf("};");
                }
                else
                {
                    fprintf(fp, "%s_%d;", treeElements[detEnd[i][0]], detEnd[i][0]);

                    printf("%s_%d;", treeElements[detEnd[i][0]], detEnd[i][0]);
                }
            }
            else if (notDeterministic[i + 1][x])
            {
                fprintf(fp, "{");

                printf("{");
                fprintf(fp, "%s_%d", treeElements[i + 1], i + 1);

                printf("%s_%d", treeElements[i + 1], i + 1);
                while (notDeterministic[i + 1][x])
                {
                    if (i + 1 != notDeterministic[i + 1][x])
                    {
                        fprintf(fp, ",");

                        printf(",");
                        fprintf(fp, " %s_%d", treeElements[notDeterministic[i + 1][x]], notDeterministic[i + 1][x]);

                        printf(" %s_%d", treeElements[notDeterministic[i + 1][x]], notDeterministic[i + 1][x]);
                    }
                    x++;
                }
                fprintf(fp, "};");

                printf("};");
            }
            else
            {
                fprintf(fp, "%s_%d;", treeElements[i + 1], i + 1);

                printf("%s_%d;", treeElements[i + 1], i + 1);
            }
        }
    }
}

void val(int pos)
{
    char *buf = malloc(sizeof(char *));
    strcpy(buf, treeElements[pos]);
    int i = 0;
    int j = 0;
    char *p = strtok(buf, "_");
    char *ptr;
    int ret;
    char *array[100];

    while (p != NULL)
    {
        array[i++] = p;
        p = strtok(NULL, "_");
    }

    for (j = 0; j < i; ++j)
    {
        ret = strtol(array[j], &ptr, 10);
        if (strcmp(ptr, "") && strcmp(array[j], "¬") && strcmp(array[j], "?") && strcmp(array[j], ">"))
            break;
    }
    if (!strcmp(array[j], "int"))
    {
        char *elem = array[j + 1];
        int min = 0;
        int max = 0;
        int posCond = 0;
        int check = 0;
        int overflow = 0;
        int underflow = 0;

        condChar[posCondChar][0] = elem;
        posCondChar++;

        for (int k = 0; k < treeTam; k++)
        {
            char *remaining = strstr(treeElements[k], elem);
            if (remaining != NULL && strstr(remaining, "=") != NULL)
            {
                char *val = malloc(sizeof(char *));
                strcpy(val, treeElements[k]);
                val = strstr(val, "=");
                val = strtok(val, "_");
                val = strtok(NULL, "_");
                char *aux = malloc(sizeof(char *));
                condCharPos[pos][posCond] = k;
                while (val != NULL)
                {
                    aux = stringMemory(aux, val);
                    val = strtok(NULL, "_");
                }
                if (check == 0)
                {
                    condChar[posCondChar - 1][1] = aux;
                    check = 1;
                }
                condNumber[posCondNumber][posCond] = aux;
                posCond++;
            }
        }
        for (int k = 0; k < posCond; k++)
        {
            if (k == 0)
            {
                min = atoi(condNumber[posCondNumber][k]);
                max = atoi(condNumber[posCondNumber][k]);
            }
            else
            {
                if (max < atoi(condNumber[posCondNumber][k]))
                    max = atoi(condNumber[posCondNumber][k]);
                else if (min > atoi(condNumber[posCondNumber][k]))
                    min = atoi(condNumber[posCondNumber][k]);
            }
            if (strcmp(condNumber[posCondNumber][k], elem) < strcmp(condNumber[posCondNumber][k], "+") && strcmp(condNumber[posCondNumber][k], elem) > 0)
            {
                if (opt[6] == 1)
                {
                    overflow = 1;
                }
                else
                {
                    max = 2147483647;
                }
            }
            else if (strcmp(condNumber[posCondNumber][k], elem) < strcmp(condNumber[posCondNumber][k], "-") && strcmp(condNumber[posCondNumber][k], elem) > 0)
            {
                if (opt[6] == 1)
                {
                    underflow = 1;
                }
                else
                {
                    min = -2147483647;
                }
            }
            int number = atoi(condNumber[posCondNumber][k]);
            if (number == 0)
            {
                char str[12];
                sprintf(str, "%d", number);
                if (strcmp(str, condNumber[posCondNumber][k]))
                {
                    if (opt[6] == 1)
                    {
                        overflow = 1;
                        underflow = 1;
                    }
                    else
                    {
                        max = 2147483647;
                        min = -2147483647;
                    }
                }
            }
        }
        posCondNumber++;
        if (opt[1] == 1)
        {

            fprintf(fp, "\n\t%s : integer;", elem);

            printf("\n\t%s : integer;", elem);
        }
        else
        {
            if (overflow == 1 && underflow == 1)
            {
                if (opt[0] == 1)
                {
                    if (opt[3] == 1)
                    {
                        if (ran[0] < 0)
                        {
                            fprintf(fp, "\n\t%s : 0..%d;", elem, ran[1]);

                            printf("\n\t%s : 0..%d;", elem, ran[1]);
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[1]);

                            printf("\n\t%s : %d..%d;", elem, ran[0], ran[1]);
                        }
                    }
                    else
                    {
                        fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[1]);

                        printf("\n\t%s : %d..%d;", elem, ran[0], ran[1]);
                    }
                }
                else
                {
                    if (opt[3] == 1)
                    {

                        fprintf(fp, "\n\t%s : 0..9223372036854775807;", elem);

                        printf("\n\t%s : 0..9223372036854775807;", elem);
                    }
                    else
                    {
                        fprintf(fp, "\n\t%s : -9223372036854775807..9223372036854775807;", elem);

                        printf("\n\t%s : -9223372036854775807..9223372036854775807;", elem);
                    }
                }
            }
            else if (overflow == 1)
            {
                if (opt[0] == 1)
                {
                    if (min < ran[0])
                    {
                        if (opt[3] == 1)
                        {
                            if (ran[0] < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..%d;", elem, ran[1]);

                                printf("\n\t%s : 0..%d;", elem, ran[1]);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[1]);

                                printf("\n\t%s : %d..%d;", elem, ran[0], ran[1]);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[1]);

                            printf("\n\t%s : %d..%d;", elem, ran[0], ran[1]);
                        }
                    }
                    else
                    {
                        if (opt[3] == 1)
                        {
                            if (min < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..%d;", elem, ran[1]);

                                printf("\n\t%s : 0..%d;", elem, ran[1]);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, min, ran[1]);

                                printf("\n\t%s : %d..%d;", elem, min, ran[1]);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, min, ran[1]);

                            printf("\n\t%s : %d..%d;", elem, min, ran[1]);
                        }
                    }
                }
                else
                {
                    if (opt[3] == 1)
                    {
                        if (min < 0)
                        {
                            fprintf(fp, "\n\t%s : 0..9223372036854775807;", elem);

                            printf("\n\t%s : 0..9223372036854775807;", elem);
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..9223372036854775807;", elem, min);

                            printf("\n\t%s : %d..9223372036854775807;", elem, min);
                        }
                    }
                    else
                    {
                        fprintf(fp, "\n\t%s : %d..9223372036854775807;", elem, min);

                        printf("\n\t%s : %d..9223372036854775807;", elem, min);
                    }
                }
            }
            else if (underflow == 1)
            {
                if (opt[0] == 1)
                {
                    if (max > ran[1])
                    {
                        if (opt[3] == 1)
                        {
                            if (ran[0] < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..%d;", elem, ran[1]);

                                printf("\n\t%s : 0..%d;", elem, ran[1]);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[1]);

                                printf("\n\t%s : %d..%d;", elem, ran[0], ran[1]);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[1]);

                            printf("\n\t%s : %d..%d;", elem, ran[0], ran[1]);
                        }
                    }
                    else
                    {
                        if (opt[3] == 1)
                        {
                            if (ran[0] < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..%d;", elem, max);

                                printf("\n\t%s : 0..%d;", elem, max);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], max);

                                printf("\n\t%s : %d..%d;", elem, ran[0], max);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], max);

                            printf("\n\t%s : %d..%d;", elem, ran[0], max);
                        }
                    }
                }
                else
                {
                    if (opt[3] == 1)
                    {
                        if (ran[0] < 0)
                        {
                            fprintf(fp, "\n\t%s : 0..%d;", elem, max);

                            printf("\n\t%s : 0..%d;", elem, max);
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : -9223372036854775807..%d;", elem, max);

                            printf("\n\t%s : -9223372036854775807..%d;", elem, max);
                        }
                    }
                    else
                    {
                        fprintf(fp, "\n\t%s : -9223372036854775807..%d;", elem, max);

                        printf("\n\t%s : -9223372036854775807..%d;", elem, max);
                    }
                }
            }
            else
            {
                if (opt[0] == 1)
                {
                    if (max > ran[1] && min < ran[0])
                    {
                        if (opt[3] == 1)
                        {
                            if (ran[0] < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..%d;", elem, ran[1]);

                                printf("\n\t%s : 0..%d;", elem, ran[1]);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[1]);

                                printf("\n\t%s : %d..%d;", elem, ran[0], ran[1]);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[1]);

                            printf("\n\t%s : %d..%d;", elem, ran[0], ran[1]);
                        }
                    }
                    else if (max > ran[1] && min > ran[1])
                    {
                        if (opt[3] == 1)
                        {
                            if (ran[1] < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..0;", elem);

                                printf("\n\t%s : 0..0;", elem);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, ran[1], ran[1]);

                                printf("\n\t%s : %d..%d;", elem, ran[1], ran[1]);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, ran[1], ran[1]);

                            printf("\n\t%s : %d..%d;", elem, ran[1], ran[1]);
                        }
                    }
                    else if (min < ran[0] && max < ran[0])
                    {
                        if (opt[3] == 1)
                        {
                            if (ran[0] < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..0;", elem);

                                printf("\n\t%s : 0..0;", elem);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[0]);

                                printf("\n\t%s : %d..%d;", elem, ran[0], ran[0]);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], ran[0]);

                            printf("\n\t%s : %d..%d;", elem, ran[0], ran[0]);
                        }
                    }
                    else if (max > ran[1])
                    {
                        if (opt[3] == 1)
                        {
                            if (min < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..%d;", elem, ran[1]);

                                printf("\n\t%s : 0..%d;", elem, ran[1]);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, min, ran[1]);

                                printf("\n\t%s : %d..%d;", elem, min, ran[1]);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, min, ran[1]);

                            printf("\n\t%s : %d..%d;", elem, min, ran[1]);
                        }
                    }
                    else if (min < ran[0])
                    {
                        if (opt[3] == 1)
                        {
                            if (ran[0] < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..%d;", elem, max);

                                printf("\n\t%s : 0..%d;", elem, max);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], max);

                                printf("\n\t%s : %d..%d;", elem, ran[0], max);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, ran[0], max);

                            printf("\n\t%s : %d..%d;", elem, ran[0], max);
                        }
                    }
                    else
                    {
                        if (opt[3] == 1)
                        {
                            if (min < 0)
                            {
                                fprintf(fp, "\n\t%s : 0..%d;", elem, max);

                                printf("\n\t%s : 0..%d;", elem, max);
                            }
                            else
                            {
                                fprintf(fp, "\n\t%s : %d..%d;", elem, min, max);

                                printf("\n\t%s : %d..%d;", elem, min, max);
                            }
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, min, max);

                            printf("\n\t%s : %d..%d;", elem, min, max);
                        }
                    }
                }
                else
                {
                    if (opt[3] == 1)
                    {
                        if (min < 0)
                        {
                            fprintf(fp, "\n\t%s : 0..%d;", elem, max);

                            printf("\n\t%s : 0..%d;", elem, max);
                        }
                        else
                        {
                            fprintf(fp, "\n\t%s : %d..%d;", elem, min, max);

                            printf("\n\t%s : %d..%d;", elem, min, max);
                        }
                    }
                    else
                    {
                        fprintf(fp, "\n\t%s : %d..%d;", elem, min, max);

                        printf("\n\t%s : %d..%d;", elem, min, max);
                    }
                }
            }
        }
    }
    else if (!strcmp(array[j], "float"))
    {
        char *elem = array[j + 1];

        int min = 0;
        int max = 0;
        int posCond = 0;
        int check = 0;
        int overflow = 0;
        int underflow = 0;

        condChar[posCondChar][0] = elem;
        posCondChar++;

        for (int k = 0; k < treeTam; k++)
        {
            if (strstr(treeElements[k], elem) != NULL && strstr(treeElements[k], "=") != NULL)
            {
                char *val = malloc(sizeof(char *));
                strcpy(val, treeElements[k]);
                val = strstr(val, "=");
                val = strtok(val, "_");
                val = strtok(NULL, "_");
                char *aux = malloc(sizeof(char *));
                condCharPos[pos][posCond] = k;
                while (val != NULL)
                {
                    aux = stringMemory(aux, val);
                    val = strtok(NULL, "_");
                }
                if (check == 0)
                {
                    condChar[posCondChar - 1][1] = aux;
                    check = 1;
                }
                condNumber[posCondNumber][posCond] = aux;
                posCond++;
            }
        }
        posCondNumber++;

        fprintf(fp, "\n\t%s : float;", elem);

        printf("\n\t%s : float;", elem);
    }
}

void valAssign(int i)
{

    fprintf(fp, "\n\tinit(%s) := ", condChar[i][0]);

    printf("\n\tinit(%s) := ", condChar[i][0]);

    if (condChar[i][1] == NULL)
    {
        fprintf(fp, "0;");

        printf("0;");
        warning = 1;
    }
    else
    {
        fprintf(fp, "%s;", condChar[i][1]);

        printf("%s;", condChar[i][1]);
    }
}

void valNext(int i)
{
    fprintf(fp, "\n\tnext(%s) := case", condChar[i][0]);

    printf("\n\tnext(%s) := case", condChar[i][0]);

    valValues(i);

    fprintf(fp, "\n\t\t\t\t\tTRUE = %s;", condChar[i][0]);

    printf("\n\t\tTRUE = %s;", condChar[i][0]);
    fprintf(fp, "\n\t\t\t\tesac;");

    printf("\n\t\tesac;");
}

void valValues(int i)
{

    int j = 0;
    while (condNumber[i][j] != NULL)
    {
        fprintf(fp, "\n\t\t\t\t\tprog = %s_%d: %s;", treeElements[condCharPos[i][j]], condCharPos[i][j], condNumber[i][j]);

        printf("\n\t\tprog = %s_%d: %s;", treeElements[condCharPos[i][j]], condCharPos[i][j], condNumber[i][j]);
        j++;
    }
}

char *stringMemory(char *aux, char *str)
{
    if (aux == NULL)
    {
        return str;
    }
    else
    {
        char *copAux = (char *)malloc(1 + strlen(aux) + strlen(str));
        strcpy(copAux, aux);
        strcat(copAux, str);
        return copAux;
    }
}

void addchild(int i)
{
    int m = 0;
    int aux = 0;
    while (notDeterministic[i][aux])
    {
        aux++;
    }
    while (notDeterministic[i][m])
    {
        int n = 0;
        while (notDeterministic[notDeterministic[i][m]][n])
        {
            int k = 0;
            int itsPresent = 0;
            while (notDeterministic[i][k])
            {
                if (notDeterministic[notDeterministic[i][m]][n] == notDeterministic[i][k])
                {
                    itsPresent = 1;
                }
                k++;
            }
            if (!itsPresent)
            {
                notDeterministic[i][aux] = notDeterministic[notDeterministic[i][m]][n];
                aux++;
            }
            n++;
        }
        m++;
    }
}
