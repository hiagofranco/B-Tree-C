#include <stdio.h>
#include <stdlib.h>
#include "Fila.h"

void criaFila(FILA *F){
    F->inicio = NULL;
}

int estaVazia(FILA *F)
{
  if(F->inicio == NULL) return 1;
  else return 0;
}

int push(FILA *F, int RRN){
    ELEMFILA *novo = (ELEMFILA*) malloc(sizeof(ELEMFILA));
    if(!novo) return 0;
    if(estaVazia(F)){
        F->inicio = novo;
        novo->RRN = RRN;
        novo->proximo = NULL;
        return 1;
    }
    else{
        ELEMFILA *p;
        p = F->inicio;
        while(p->proximo != NULL){
            p = p->proximo;
        }
        p->proximo = novo;
        novo->proximo = NULL;
        novo->RRN = RRN;
        return 1;
    }
}

int pop(FILA *F, int *RRN){
    if(estaVazia(F)){
        return 0;
    }
    ELEMFILA *p;
    p = F->inicio;
    *RRN = p->RRN;
    F->inicio = p->proximo;
    free(p);
    return 1;
}
