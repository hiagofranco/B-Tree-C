#include <stdio.h>
#include <stdlib.h>
#include "Fila.h"
#include "B-Tree.h"
#include "Registro.h"
#include "Log.h"

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
int buscaBT(FILE *arq, int offset, int id, long int *offset_encontrado) {

  if(offset == -1)
    return FALSE;

  else {

    PAGINA p;
    int pos = 0;

    fseek(arq, sizeof(CABECALHO_BTREE) + offset*sizeof(PAGINA), SEEK_SET); //Vai para a pagina raiz.
    fread(&p, sizeof(PAGINA), 1, arq); //Le os dados da pagina raiz.

    int i;
    for(i = 0; i < p.numeroChaves; i++){
        if(id == p.chaves[i].id){
            *offset_encontrado = p.chaves[i].offset;
            return TRUE;
        }
        if(id < p.chaves[i].id){
            pos = i;
            break;
        }
    }

    if(i == p.numeroChaves){
        pos = i;
    }

    return buscaBT(arq, p.filhos[pos], id, offset_encontrado);
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

int inserirBT(FILE *arq, FILE *logTxt, int offset, CHAVE *chave, CHAVE *chavePromovida, int *direitoChavePromovida, int *contadorDePaginas) {
  PAGINA p, newP;
  int i;
  int pos = 0;

  if(offset == -1) { //Chegamos em um no folha.
    chavePromovida = chave;
    *direitoChavePromovida = -1;
    return PROMOTION;
  }
  else {

    //Aqui pegamos as informacoes de uma pagina no disco e passamos para a memoria primaria para trabalharmos em cima delas
    fseek(arq, sizeof(CABECALHO_BTREE) + offset*sizeof(PAGINA), SEEK_SET);
    fread(&p, sizeof(PAGINA), 1, arq);

    /* Aqui procuramos a posicao na qual a chave deveria estar e, entao, checamos
    se ela realmente ja existe. */
    for(i = 0; i < p.numeroChaves; i++){
        if(chave->id == p.chaves[i].id){
            return ERROR;
        }
        if(chave->id < p.chaves[i].id){
            pos = i;
            break;
        }
    }

    if(i == p.numeroChaves){
        pos = i;
    }

    /*Chamada de recursao com o objetivo de se atingir os nos folha da arvore. */
    int return_value = inserirBT(arq, logTxt, p.filhos[pos], chave, chavePromovida, direitoChavePromovida, contadorDePaginas);

    if(return_value == NO_PROMOTION || return_value == ERROR)
      return return_value;

    /* Aqui, se tiver espaco no noh para inserir uma nova chave, entao fazemos uma Insercao
    em vetor ordenado. Se nao, dividimos o no.*/

    else if(p.numeroChaves < ORDEM-1) { //Checa se ainda ha espaco.
      p.numeroChaves++;
      int posOrd, fim;
      posOrd = -1;
      fim = ORDEM-1;
      /* Comeco do algoritmo de insercao em vetor ordenado. */
      for(i = 0; i < ORDEM-1; i++) {
        if(p.chaves[i].id == -1){
          fim = i;
          break;
        }
      }

      if(p.chaves[fim-1].id < chave->id)
        posOrd = fim;

      else{
        for(i = 0; i < ORDEM-1; i++) {
          if(p.chaves[i].id != -1 && p.chaves[i].id >= chave->id) {
            posOrd = i;
            break;
          }
        }
      }

      for(i = fim; i >= posOrd; i--){
        if(i + 1 != ORDEM - 1){
            p.chaves[i+1] = p.chaves[i];
        }
      }
      p.chaves[posOrd] = *chave;


      for(i = fim; i >= posOrd; i--){
        p.filhos[i+1] = p.filhos[i];
      }
      p.filhos[posOrd + 1] = *direitoChavePromovida;
      /* Fim do algoritmo de insercao em vetor ordenado e organizacao dos filhos. */

      //Aqui escrevemos a informacao organizada em memoria RAM para o disco
      fseek(arq, sizeof(CABECALHO_BTREE) + (p.RRNDaPagina)*sizeof(PAGINA), SEEK_SET);
      fwrite(&p, sizeof(PAGINA), 1, arq);
      return NO_PROMOTION;
    }

    else { //Insercao de chave com particionamento.
      split(arq, chave->id, chave->offset, &p, chavePromovida, direitoChavePromovida, &newP, contadorDePaginas, p.RRNDaPagina);

      //Aqui procuramos o lugar correto do arquivo para escrevermos as informacoes
      fseek(arq, sizeof(CABECALHO_BTREE) + (p.RRNDaPagina)*sizeof(PAGINA), SEEK_SET);
      fwrite(&p, sizeof(PAGINA), 1, arq);
      fseek(arq, sizeof(CABECALHO_BTREE) + (newP.RRNDaPagina)*sizeof(PAGINA), SEEK_SET);
      fwrite(&newP, sizeof(PAGINA), 1, arq);

      //Temos que trocar a chave pela chavePromovida pois esta passa a ser a nova chave que deve ser inserida em um no
      *chave = *chavePromovida;
      //Aqui modificamos o valor do filho direito pelo RRN da nova pagina criada
      *direitoChavePromovida = newP.RRNDaPagina;

      log_insercao(logTxt, NULL, 1, &p, NULL);
      log_insercao(logTxt, chavePromovida, 2, NULL, NULL);
      return PROMOTION;
    }
  }
}

void split(FILE *arq, int i_key, int i_offset, PAGINA *p, CHAVE *promo_key, int *promo_r_child, PAGINA *newP, int *contadorDePaginas, int RRNPaginaSplitada) {
  PAGINA_SPLIT pSplit;

  int i;
  for (i = 0; i < ORDEM-1; i++) {// Copia todas as chaves para a nova pagina.
    pSplit.chaves[i] = p->chaves[i];
  }

  for (i = 0; i < ORDEM; i++) {// Copia todas os filhos para a nova pagina.
    pSplit.filhos[i] = p->filhos[i];
  }

  /* Aqui colocamos a ultima posicao como invalida, ja que iremos inserir uma nova chave */
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

    for(i = fim; i >= posOrd; i--){
        if(i + 1 != ORDEM){
            pSplit.chaves[i+1] = pSplit.chaves[i];
        }
    }

  pSplit.chaves[posOrd].id = i_key;
  pSplit.chaves[posOrd].offset = i_offset;
  /* Fim do algoritmo de insercao em vetor ordenado. */

  for(i = fim; i >= posOrd; i--){
    pSplit.filhos[i+1] = pSplit.filhos[i];
  }

  //se a pagina nao for folha, temos que colocar o filho certo
  if(pSplit.filhos[posOrd] != -1){
    if(*promo_r_child == -1){
        pSplit.filhos[posOrd + 1] = RRNPaginaSplitada;
    }
    else{
        pSplit.filhos[posOrd + 1] = *promo_r_child;
    }
  }

  //Aqui verificamos se a ORDEM eh par ou impar para fazermos o split correto
  //Abaixo esta o split para arvores de ordem impar (resto da divisao por 2 eh diferente de zero)
  if(ORDEM % 2 != 0){
    /* Copia as chaves e os filhos de Psplit para P ate a chave promovida */
    for(i = 0; i < ORDEM/2; i++)
        p->chaves[i] = pSplit.chaves[i];

    for(i = ORDEM/2; i < ORDEM-1; i++) {
        p->chaves[i].id = -1;
        p->chaves[i].offset = -1;
    }

    for(i = 0; i < (ORDEM + 1)/2; i++)
        p->filhos[i] = pSplit.filhos[i];

    for(i = (ORDEM + 1)/2; i < ORDEM; i++)
        p->filhos[i] = -1;

    /* Copia as chaves e os filhos de Psplit para newP a partir da chave promovida */
    for(i = ORDEM/2 + 1; i < ORDEM; i++){
        newP->chaves[i - ORDEM/2 - 1] = pSplit.chaves[i];
    }

    for(i = ORDEM/2; i < ORDEM-1; i++) {
        newP->chaves[i].id = -1;
        newP->chaves[i].offset = -1;
    }

    for(i = (ORDEM + 1)/2; i < ORDEM + 1; i++){
        newP->filhos[i - (ORDEM + 1)/2] = pSplit.filhos[i];
    }

    for(i = (ORDEM + 1)/2; i < ORDEM; i++)
        newP->filhos[i] = -1;

    newP->numeroChaves = ORDEM/2;
    p->numeroChaves = ORDEM/2;
    newP->RRNDaPagina = *contadorDePaginas;
    (*contadorDePaginas)++;

    *promo_key = pSplit.chaves[ORDEM/2];

  }

  //Abaixo esta o split para arvores de ordem par (resto da divisao por 2 eh zero)
  else{
    /* Copia as chaves e os filhos de Psplit para P ate a chave promovida */
    for(i = 0; i < ORDEM/2 - 1; i++)
        p->chaves[i] = pSplit.chaves[i];

    for(i = ORDEM/2 - 1; i < ORDEM-1; i++) {
        p->chaves[i].id = -1;
        p->chaves[i].offset = -1;
    }

    for(i = 0; i < (ORDEM)/2; i++)
        p->filhos[i] = pSplit.filhos[i];

    for(i = (ORDEM)/2; i < ORDEM; i++)
        p->filhos[i] = -1;

    /* Copia as chaves e os filhos de Psplit para newP a partir da chave promovida */
    for(i = ORDEM/2; i < ORDEM; i++){
        newP->chaves[i - ORDEM/2] = pSplit.chaves[i];
    }

    for(i = ORDEM/2; i < ORDEM-1; i++) {
        newP->chaves[i].id = -1;
        newP->chaves[i].offset = -1;
    }

    for(i = (ORDEM)/2; i < ORDEM + 1; i++){
        newP->filhos[i - (ORDEM)/2] = pSplit.filhos[i];
    }

    for(i = (ORDEM)/2 + 1; i < ORDEM; i++)
        newP->filhos[i] = -1;

    newP->numeroChaves = ORDEM/2;
    p->numeroChaves = ORDEM/2 - 1;
    newP->RRNDaPagina = *contadorDePaginas;
    (*contadorDePaginas)++;

    *promo_key = pSplit.chaves[ORDEM/2 - 1];

  }

  /* Recebe o endereco da nova pagina que esta a direta */
  *promo_r_child = newP->RRNDaPagina;
}

void desatualizarBTree(FILE *index, CABECALHO_BTREE cabecalho){

    cabecalho.estaAtualizada = FALSE;
    fseek(index, 0, SEEK_SET);
    fwrite(&cabecalho, sizeof(CABECALHO_BTREE), 1, index);

}

void atualizarBTree(FILE *index, CABECALHO_BTREE cabecalho){

    cabecalho.estaAtualizada = TRUE;
    fseek(index, 0, SEEK_SET);
    fwrite(&cabecalho, sizeof(CABECALHO_BTREE), 1, index);

}

void exibirBT(FILE *index, FILE *logTxt, int root){
    FILA F;
    PAGINA p;
    criaFila(&F);
    int RRNAtual, i;

    //filhosSuperior eh a quantidade de filhos que todo o nivel acima do atual tem (ou seja, a quantidade de paginas em um nivel)
    //filhosInferior representa o total de filhos que o nivel atual tem
    //usa-se essas variaveis para controlar o nivel em que estamos realizando a leitura
    int filhosSuperior, filhosInferior;

    //primeiro nivel eh o 0
    int nivel = 0;

    //num primeiro momento pegamos os filhos da raiz e passamos para uma fila para percorrermos a arvore em largura
    fseek(index, sizeof(CABECALHO_BTREE) + root*sizeof(PAGINA), SEEK_SET);
    fread(&p, sizeof(PAGINA), 1, index);
    //A raiz nao eh filha de ninguem
    filhosSuperior = 0;
    //Valor default para essa variavel
    filhosInferior = 0;

    //Aqui pegamos todos os filhos dessa pagina e aumentamos a quantidade de filhos do nivel atual
    for(i = 0; i < ORDEM; i++){
        if(p.filhos[i] != -1){
            push(&F, p.filhos[i]);
                filhosInferior++;
        }
    }
    //funcao que escreve no arquivo de log a pagina atual
    log_exibirBTree(logTxt, p, nivel);

    //depois de analisar a raiz aumentamos o nivel (descemos na arvore)
    nivel++;

    //como estamos num nivel abaixo agora, o nivel atual, inicialmente nao temos filhos e a quantidade de paginas no nivel atual
    //eh igual a filhosInferior  do nivel acima
    filhosSuperior = filhosInferior;
    filhosInferior = 0;

    //enquanto a fila com os filhos das paginas nao estiver vazia, analisamos as paginas restantes com busca em largura
    while(!estaVazia(&F)){

        //aqui retiramos um dos filhos colocados na fila e analisamos a pagina, realizando um procedimento semelhante ao feito na raiz
        pop(&F, &RRNAtual);
        fseek(index, sizeof(CABECALHO_BTREE) + RRNAtual*sizeof(PAGINA), SEEK_SET);
        fread(&p, sizeof(PAGINA), 1, index);
        for(i = 0; i < ORDEM; i++){
           if(p.filhos[i] != -1){
                push(&F, p.filhos[i]);
                filhosInferior++;
           }
        }

        log_exibirBTree(logTxt, p, nivel);

        //deve-se decrementar filhosSuperior pois terminamos de analisar uma pagina do nivel atual
        filhosSuperior--;

        //se filhosSuperior for zero, significa que acabamos de analisar as paginas de um nivel,
        //agora devemos descer um nivel na arvore
        if(filhosSuperior == 0){
            nivel++;
            filhosSuperior = filhosInferior;
            filhosInferior = 0;
        }
    }
}
