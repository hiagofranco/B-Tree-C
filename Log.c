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
        fprintf(arq, "Execucao de operacao de INSERCAO de <%d>, <%s>, <%s>.\r\n", chave->id, reg->titulo, reg->genero);
        break;
      case 1: //Divisao de no.
        fprintf(arq, "Divisao de no - pagina %d\r\n", p->RRNDaPagina);
        break;
      case 2: //Chave promovida
        fprintf(arq, "Chave <%d> promovida\r\n", chave->id);
        break;
      case 3: //Chave inserida
        fprintf(arq, "Chave <%d> inserida com sucesso\r\n", reg->id);
        break;
      case 4: //Chave duplicada
        fprintf(arq, "Chave <%d> duplicada\r\n", chave->id);
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
    fprintf(arq, "Execucao da criacao do arquivo de indice <arvore.idx> com base no arquivo de dados <dados.dad>\r\n");
    fclose(arq);
}

void log_busca(FILE *arq, int id) {
  arq = fopen("log_HMoreira.txt", "a");
  if(!arq) {
      printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
      exit(1);
  }
    fprintf(arq, "Execucao de operacao de PESQUISA de <%d>\r\n", id);
    fclose(arq);
}

void log_sucessoBusca(FILE *arq, REGISTRO r, long int offset){
    arq = fopen("log_HMoreira.txt", "a");
    if(!arq) {
      printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
      exit(1);
    }
    fprintf(arq, "Chave <%d> encontrada, offset <%ld>, Titulo: <%s>, Genero: <%s>\r\n", r.id, offset, r.titulo, r.genero);
    fclose(arq);
}

void log_falhaBusca(FILE *arq, int id){
    arq = fopen("log_HMoreira.txt", "a");
    if(!arq) {
      printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
      exit(1);
    }
    fprintf(arq, "Chave <%d> nao encontrada\r\n", id);
    fclose(arq);
}

void log_funcaoExibirCabecalho(FILE *arq) {
    arq = fopen("log_HMoreira.txt", "a");
    if(!arq) {
        printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
        exit(1);
    }
    fprintf(arq, "Execucao de operacao para mostrar a arvore-B gerada:\r\n");
    fclose(arq);
}

void log_exibirBTree(FILE *log, PAGINA p, int nivel){
    log = fopen("log_HMoreira.txt", "a");
    int i;
    if(!log) {
        printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
        exit(1);
    }
    fprintf(log, "%d  %d  ", nivel, p.numeroChaves);
    for(i = 0; i < p.numeroChaves; i++){
        fprintf(log, "<%d/%ld>    ", p.chaves[i].id, p.chaves[i].offset);
    }
    fprintf(log, "\r\n");
    fclose(log);
}

