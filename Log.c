#include <stdio.h>
#include <stdlib.h>
#include "Log.h"
#include "Registro.h"

void log_insercao(FILE *arq, CHAVE chave, FILE *dados, int codigo, pagina *p) {
  arq = fopen("log_HMoreira.txt", "a");
  if(!arq) {
      printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)");
      exit(1);
  }

  else {
    switch (codigo) {
      case 0: // Execucao da insercao.
        dados = fopen("dados.dad", "rb");
        if(!dados) {
          printf("Erro ao abrir o arquivo de dados! (dados.dad)\n");
          exit(1);
        }
        fseek(dados, chave.offset, SEEK_SET);

        char size, buffer[1000]; REGISTRO r;

        fread(&size, sizeof(size), 1, dados);
        fread(buffer, size, 1, dados);

        int pos = 0;
        sscanf(parser(buffer, &pos), "%d", &r.id);
        strcpy(r.titulo, parser(buffer, &pos));
        strcpy(r.genero, parser(buffer, &pos));

        fprintf(arq, "Execucao de operacao de INSERCAO de %d, %s, %s.\n", chave.id, r.titulo, r.genero);
        fclose(dados);
        break;
      case 1: //Divisao de no.
        fprintf(arq, "Divisao de no - pagina %d\n", *p.RRNDaPagina);
        break;
      case 2: //Chave promovida
        fprintf(arq, "Chave %d promovida\n", chave.id);
        break;
      case 3: //Chave inserida
        fprintf(arq, "Chave %d inserida com sucesso\n", chave.id);
        break;
      case 4: //Chave duplicada
        fprintf(arq, "Chave %d duplicada\n", chave.id);
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
    fprintf(arq, "Execucao da criacao do arquivo de indice <arvore.idx> com base no arquivo de dados <dados.dad>\n");
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
