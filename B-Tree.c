#include <stdio.h>
#include <stdlib.h>
#include "B-Tree.h"

void criaBT(FILE *arq) {
  /*Primeiramente criamos o cabecalho do arquivo e inserimos no arquivo de indice */
  PAGINA raiz;

  raiz.RRNDaPagina = 0;
  raiz.numeroChaves = 0;

  int i;
  for (i = 0; i < ORDEM; i++)
    raiz.filhos[i] = -1;

  for (i = 0; i < ORDEM-1; i++) {
    raiz.chaves[i].id = -1;
    raiz.chaves[i].offset = -1;
  }

 // raiz.isFolha = TRUE;

  CABECALHO_BTREE cabecalho;
  cabecalho.noRaiz = 0;
  cabecalho.estaAtualizada = TRUE;
  cabecalho.contadorDePaginas = 1;

  arq = fopen("arvore.idx", "wb");
  fwrite(&cabecalho, sizeof(CABECALHO_BTREE), 1, arq);
  fwrite(&raiz, sizeof(PAGINA), 1, arq);
  fclose(arq);
}

/* Recebe primeiro o offset do no raiz */
int buscaBT(FILE *arq, int offset, int chave, int offset_encontrado, int pos_encontrada) {

  if(offset == -1)
    return FALSE;

  else {

    PAGINA p;
    int pos = 0;

    fseek(arq, offset*sizeof(PAGINA), SEEK_SET); //Vai para a pagina raiz.
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
      offset_encontrado = offset;
      return TRUE;
    }
    else
      return buscaBT(arq, p.filhos[pos], chave, offset_encontrado, pos_encontrada);
  }
}

/* offset = p�gina da �rvore-B que est� atualmente em uso (inicialmente, a raiz).
 * chave = a chave a ser inserida.
 * chavePromovida = retorna a chave promovida, caso a inser��o resulte no
 * particionamento e na promo��o da chave (parametro de recursao).
 * direitoChavePromovida = retorna o ponteiro para o filho direito de chavePromovida
 * (parametro de recursao).
 * Inicializa passand a no raiz e valores invalidos (-1) para os parametros de recursao.
*/

int inserirBT(FILE *arq, int offset, CHAVE chave, CHAVE chavePromovida, int direitoChavePromovida, int *contadorDePaginas) {
  PAGINA p, newP;
  int i;
  int pos = 0;

  if(offset == -1) { //Chegamos em um no folha.
    chavePromovida = chave;
    direitoChavePromovida = -1;
    return PROMOTION;
  }
  else {


    fseek(arq, sizeof(CABECALHO_BTREE) + offset*sizeof(PAGINA), SEEK_SET);
    fread(&p, sizeof(PAGINA), 1, arq);

    int caralha;/*
    for(caralha = 0; caralha < ORDEM; caralha++){
        printf("filho%d = %d   ", caralha, p.filhos[caralha]);
    }*/

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
      return ERROR; //chave duplicada!

    /*Chamada de recursao com o objetivo de se atingir os nos folha da arvore. */
    int return_value = inserirBT(arq, p.filhos[pos], chave, chavePromovida, direitoChavePromovida, contadorDePaginas);

    if(return_value == NO_PROMOTION || return_value == ERROR)
      return return_value;

    /* Aqui, se tiver espaco no noh para inserir uma nova, entao fazemos uma Insercao
    em vetor ordenado. Se nao, dividimos o no.*/

    else if(p.numeroChaves < ORDEM-1) { //Checa se ainda ha espaco.
      p.numeroChaves++;
      printf("\n chegou na insercao ordenada");
      int posOrd, fim;
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

    for(caralha = 0; caralha < ORDEM -1; caralha++){
        printf("chave%d = %d   ", caralha, p.chaves[caralha].id);
    }
      /* Fim do algoritmo de insercao em vetor ordenado. */

      fseek(arq, -sizeof(PAGINA), SEEK_CUR);
      fwrite(&p, sizeof(PAGINA), 1, arq);
      return NO_PROMOTION;
    }

    else { //Insercao de chave com particionamento.
            printf("SPLIT ");
      split(arq, chave.id, chave.offset, &p, chavePromovida, direitoChavePromovida, &newP, contadorDePaginas);
      fwrite(&p, sizeof(PAGINA), 1, arq);
      fwrite(&newP, sizeof(PAGINA), 1, arq);
      return PROMOTION;
    }
  }
}

void split(FILE *arq, int i_key, int i_offset, PAGINA *p, CHAVE promo_key, int promo_r_child, PAGINA *newP, int *contadorDePaginas) {
  PAGINA_SPLIT pSplit;

  int i;
  for (i = 0; i < ORDEM-1; i++) {// Copia todas as chaves e filhos para a nova pagina.
    pSplit.chaves[i] = p->chaves[i];
    pSplit.filhos[i] = p->filhos[i];
  }
  /* Aqui colocamos a ultiam posicao como invalida, ja que iremos inserir uma nova chave */
  pSplit.chaves[ORDEM-1].id = -1;
  pSplit.chaves[ORDEM-1].offset = -1;
  pSplit.filhos[ORDEM] = -1;

  /* Agora inserimos a nova chave e offset nesta pagina (insercao ordenada). */
  int posOrd, fim;
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

  /* Aqui pego a chave do meio do vetor. Se ele for impar, tudo certo. Se ele for par,
   * irei pegar a chave a direita, por isso o ORDEM/2 + 1 */
  promo_key = pSplit.chaves[ORDEM/2];

  /* Recebe o endereco da nova pagina que esta a direta */
  promo_r_child = newP->RRNDaPagina;

  /* Copia as chaves e os filhos de Psplit para P ate a chave promovida */
  for(i = 0; i < ORDEM/2; i++)
    p->chaves[i] = pSplit.chaves[i];

  for(i = 0; i < ORDEM/2; i++)

  for(i = ORDEM/2; i < ORDEM-1; i++) {
    p->chaves[i].id = -1;
    p->chaves[i].offset = -1;
  }

  /* Copia as chaves e os filhos de Psplit para P a partir da chave promovida */
  for(i = ORDEM/2 + 1; i < ORDEM; i++)
    newP->chaves[i - ORDEM/2 + 1] = pSplit.chaves[i];

  for(i = ORDEM/2; i < ORDEM-1; i++) {
    newP->chaves[i].id = -1;
    newP->chaves[i].offset = -1;
  }

  //Preciso lembrar de att o cabecalho depois.
  (*contadorDePaginas)++;
  newP->RRNDaPagina = *contadorDePaginas;
}

//funcao que adquire o RRN do no raiz
int buscaRaiz(FILE *arq){
    CABECALHO_BTREE cabecalho;
    fseek(arq, 0, 0);
    fread(&cabecalho, sizeof(CABECALHO_BTREE), 1, arq);
    return cabecalho.noRaiz;
}

void ler_btree(FILE *arq){
    arq = fopen("arvore.idx", "rb");
    fseek(arq, 0, 0);
    CABECALHO_BTREE cabecalho;
    PAGINA pag;
    int i;
    fread(&cabecalho, sizeof(CABECALHO_BTREE), 1, arq);
    printf("\nnoRaiz = %d   contadordePaginas = %d", cabecalho.noRaiz, cabecalho.contadorDePaginas);
    while(fread(&pag, sizeof(PAGINA), 1, arq)){
        printf("\n\nRRNDaPagina = %d", pag.RRNDaPagina);
        printf("\nnumeroChaves = %d", pag.numeroChaves);
        i = 0;
        while(i < ORDEM - 1 )
            printf("chaves[%d]: (ID = %d  Off = %d)   ", i, pag.chaves[i].id, pag.chaves[i].offset);
            i++;
        }
    }
/*
void ler_criacao_btree(FILE *arq){
    CABECALHO_BTREE cabecalho;
    PAGINA pag;
    fread(&cabecalho, sizeof(CABECALHO_BTREE), 1, arq);
    fread(&pag, sizeof(PAGINA), 1, arq);
    printf(" raiz: %d   estaAtualizada: %d\n ", cabecalho.noRaiz, cabecalho.estaAtualizada);
    printf(" contador: %d  numeroChaves: %d  1filho: %d  isFolha: %d\n", pag.RRNDaPagina, pag.numeroChaves, pag.filhos[0], pag.isFolha);
}*/

