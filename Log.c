#include <stdio.h>
#include <stdlib.h>
#include "Registro.h"
#include "B-Tree.h"
#include "Log.h"

void log_insercao(FILE *arq, CHAVE *chave, int codigo, PAGINA *p, REGISTRO *reg) {
  arq = fopen("log_HMoreira.txt", "a");
  if(!arq) {
      printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
      exit(1);
  }

  else {
    switch (codigo) {
      case 0: // Execucao da insercao.
        fprintf(arq, "Execucao de operacao  de INSERCAO de %d, %s, %s.\n", chave->id, reg->titulo, reg->genero);
        break;
      case 1: //Divisao de no.
        fprintf(arq, "Divisao de no - pagina %d\n", p->RRNDaPagina);
        break;
      case 2: //Chave promovida
        fprintf(arq, "Chave %d promovida\n", chave->id);
        break;
      case 3: //Chave inserida
        fprintf(arq, "Chave %d inserida com sucesso\n", reg->id);
        break;
      case 4: //Chave duplicada
        fprintf(arq, "Chave %d duplicada\n", chave->id);
        break;
    }
    fclose(arq);
  }
}

void log_indice(FILE *arq) {
    arq = fopen("log_HMoreira.txt", "a");
    if(!arq) {
        printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
        exit(1);
    }
    fprintf(arq, "Execucao da criacao do arquivo de indice arvore.idx com base no arquivo de dados dados.dad\n");
    fclose(arq);
}

void log_busca(FILE *arq, CHAVE chave) {
  arq = fopen("log_HMoreira.txt", "a");
  if(!arq) {
      printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
      exit(1);
  }
    fprintf(arq, "Execucao de operacao de PESQUISA de %d\n", chave.id);
    fclose(arq);
}
