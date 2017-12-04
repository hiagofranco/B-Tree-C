#ifndef FILA_H_
#define FILA_H_

typedef struct elemFila {
    int RRN;
    struct elemFila *proximo;
}ELEMFILA;

typedef struct fila {
    ELEMFILA *inicio;
} FILA;

void criaFila(FILA *F);
int estaVazia(FILA *F);
int push(FILA *F, int RRN);
int pop(FILA *F, int *RRN);
#endif //FILA_H_
