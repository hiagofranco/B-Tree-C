#include <stdio.h>
#include <stdlib.h>
#include "B-Tree.h"

void criaBT(FILE *arq) {
  /*Primeiramente criamos o cabecalho do arquivo e inserimos no arquivo de indice */
  PAGINA raiz;

  raiz.contadorPagina = 0;
  raiz.numeroChaves = 0;

  int i;
  for (i = 0; i < ORDEM; i++)
    raiz.filhos[i] = -1;

  for (i = 0; i < ORDEM-1; i++) {
    raiz.chaves[i].id = -1;
    raiz.chaves[i].offset = -1;
  }

  raiz.isFolha = TRUE;

  CABECALHO_BTREE cabecalho;
  cabecalho.noRaiz = 0;
  cabecalho.estaAtualizada = TRUE;

  fwrite(&cabecalho, sizeof(CABECALHO_BTREE), 1, arq);
  fwrite(&raiz, sizeof(PAGINA), 1, arq);
}

/* Recebe primeiro o RRN do no raiz */
int buscaBT(FILE *arq, int RRN, int chave, int RRN_encontrado, int pos_encontrada) {

  if(RRN == -1)
    return FALSE;

  else {

    PAGINA p;
    int pos = 0;

    fseek(arq, RRN*sizeof(PAGINA), SEEK_SET); //Vai para a pagina raiz.
    fread(&p, sizeof(PAGINA), 1, arq); //Le os dados da pagina raiz.

    int i;
    /* Se a chave for maior que o ultimo elemento, entao a posicao ja devera ser o
    ultimo elemento. (Lembre-se que o tamanho do vetor eh ORDEM-1, por isso o ultimo
    elemento esta em ORDEM-2) */
    if(chave > p.chaves[ORDEM-2].id)
      pos = ORDEM-2;
    else
      for (i = 0; i < ORDEM-2; i++) //Aqui como ja foi verifcado a ultima pos, entao ORDEM-2.
        if(chave > p.chaves[i].id)
          pos++;

    if(chave == p.chaves[pos].id) {
      pos_encontrada = pos;
      RRN_encontrado = RRN;
      return TRUE;
    }
    else
      return buscaBT(arq, p.filhos[pos], chave, RRN_encontrado, pos_encontrada);
  }
}

/* offset = página da árvore-B que está atualmente em uso (inicialmente, a raiz).
 * chave = a chave a ser inserida.
 * chavePromovida = retorna a chave promovida, caso a inserção resulte no
 * particionamento e na promoção da chave (parametro de recursao).
 * direitoChavePromovida = retorna o ponteiro para o filho direito de chavePromovida
 * (parametro de recursao).
*/

int inserirBT(FILE *arq, int offset, CHAVE chave, int chavePromovida, int direitoChavePromovida) {
  PAGINA p, newP; int i; int pos = 0;

  if(offset == -1) { //Chegamos em um no folha.
    chavePromovida = chave.id;
    direitoChavePromovida = -1;
    return PROMOTION;
  }
  else {

    fseek(arq, sizeof(CABECALHO_BTREE) + offset*sizeof(PAGINA), SEEK_SET);
    fread(&p, sizeof(PAGINA), 1, arq);

    /* Aqui procuramos a posicao na qual a chave deveria estar e, entao, checamos
    se ela realmente ja existe. */
    if(chave.id > p.chaves[ORDEM-2].id)
      pos = ORDEM-2;
    else {
      for (i = 0; i < ORDEM-2; i++) //Aqui como ja foi verifcado a ultima pos, entao ORDEM-2.
        if(chave.id > p.chaves[i].id)
          pos++;
    }

    /* Se a chave ja existe, entao retornamos um erro, visto que nao aceitamos chaves
    duplicados */
    if(chave.id == p.chaves[pos].id)
      return ERRO; //chave duplicada!

    /*Chamada de recursao com o objetivo de se atingir os nos folha da arvore. */
    int return_value = inserirBT(arq, p.filhos[pos], chave, chavePromovida, direitoChavePromovida);

    if(return_value == NO_PROMOTION || return_value == ERRO)
      return return_value;

    /* Aqui, se tiver espaco no noh para inserir uma nova, entao fazemos uma Insercao
    em vetor ordenado. Se nao, dividimos o no.*/
    else if(p.numeroChaves < ORDEM-1) { //Checa se ainda ha espaco.
      int posOrd, fim;
      //int aux;
      posOrd = -1;
      fim = ORDEM-1; //Verificar se eh inutil no futuro.

      /* Comeco do algoritmo de insercao em vetor ordenado. */
      for(i = 0; i < ORDEM-1; i++) {
        if(p.chaves[i].id == -1){
          fim = i;
          break;
        }
      }

      if(p.chaves[fim-1].id < chave.id)
        posOrd = fim;

      else{
        for(i = 0; i < ORDEM-1; i++) {
          if(p.chaves[i].id != -1 && p.chaves[i].id >= chave.id) {
            posOrd = i;
            break;
          }
        }
      }

      for(i = fim; i >= posOrd; i--)
        p.chaves[i+1].id = p.chaves[i].id;

      p.chaves[posOrd].id = chave.id;
      p.chaves[posOrd].offset = chave.offset;
      p.numeroChaves++;
      /* Fim do algoritmo de insercao em vetor ordenado. */
      fwrite(&p, sizeof(PAGINA), 1, arq);

      return NO_PROMOTION;
    }

    else { //Insercao de chave com particionamento.
      split(arq, chave.id, chave.offset, p, chavePromovida, direitoChavePromovida, newP);
      fwrite(&p, sizeof(PAGINA), 1, arq);
      fwrite(&newP, sizeof(PAGINA), 1, arq);
      return PROMOTION;
    }
  }
}

void split(FILE *arq, int i_key, int i_offset, PAGINA p, int promo_key, int promo_r_child, PAGINA newp) {
  PAGINA_SPLIT pSplit;

  int i;
  for (i = 0; i < ORDEM-1; i++) {// Copia todas as chaves e filhos para a nova pagina.
    pSplit.chaves[i] = p.chaves[i];
    pSplit.filhos[i] = p.filhos[i];
  }
  /* Aqui colocamos a ultiam posicao como invalida, ja que iremos inserir uma nova chave */
  pSplit.chaves[ORDEM-1].id = -1;
  pSplit.chaves[ORDEM-1].offset = -1;
  pSplit.filhos[ORDEM] = -1;

  /* Agora inserimos a nova chave e offset nesta pagina (insercao ordenada). */
  int posOrd, fim;
  //int aux;
  posOrd = -1;
  fim = ORDEM; //Verificar se eh inutil no futuro.

  /* Comeco do algoritmo de insercao em vetor ordenado. */
  for(i = 0; i < ORDEM; i++) {
    if(pSplit.chaves[i].id == -1){
      fim = i;
      break;
    }
  }

  if(pSplit.chaves[fim-1].id < i_key)
    posOrd = fim;

  else{
    for(i = 0; i < ORDEM; i++) {
      if(pSplit.chaves[i].id != -1 && pSplit.chaves[i].id >= i_key) {
        posOrd = i;
        break;
      }
    }
  }

  for(i = fim; i >= posOrd; i--)
    pSplit.chaves[i+1].id = pSplit.chaves[i].id;

  pSplit.chaves[posOrd].id = i_key;
  pSplit.chaves[posOrd].offset = i_offset;
  /* Fim do algoritmo de insercao em vetor ordenado. */

}

//função que adquire o RRN do nó raiz
int buscaRaiz(FILE *arq){
    CABECALHO_BTREE cabecalho;
    fseek(arq, 0, 0);
    fread(&cabecalho, sizeof(CABECALHO_BTREE), 1, arq);
    return cabecalho.noRaiz;
}

void ler_criacao_btree(FILE *arq){
    CABECALHO_BTREE cabecalho;
    PAGINA pag;
    fread(&cabecalho, sizeof(CABECALHO_BTREE), 1, arq);
    fread(&pag, sizeof(PAGINA), 1, arq);
    printf(" raiz: %d   estaAtualizada: %d\n ", cabecalho.noRaiz, cabecalho.estaAtualizada);
    printf(" PaginaAtual: %d  numeroChaves: %d  1chave: %d  isFolha: %d\n", pag.contadorPagina, pag.numeroChaves, pag.chaves[0].id, pag.isFolha);
}
