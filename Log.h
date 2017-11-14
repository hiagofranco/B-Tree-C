#include<stdio.h>
#include<stdlib.h>

void log_insercao(FILE *arq,int id,char *titulo,char *genero);
void log_busca(FILE *arq,int id,char *titulo,char *genero,int offset);
void log_remocao(FILE *arq,int id);
void log_mostrarArvore(FILE *arq,int id);
void log_indice(FILE *arq);
