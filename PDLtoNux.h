#ifndef PDLTONUX_H_
#define PDLTONUX_H_

#include "Converter.h"
#include <string.h>

void conv           (No *no, int option[7], char *smv, int range[2]);
void mod            (No *no);
void var            (No *no);
No* prog           (No *no);
void progvalue      (Nolist *nolist);
void Assign         ();
void firstElement   (No *no);
void elements       ();
void next           ();
void val            ();
void valAssign      (int i);
void valNext        (int i);
void valValues      (int i);
char* stringMemory  (char *aux, char *str);
void addchild       (int i);
#endif