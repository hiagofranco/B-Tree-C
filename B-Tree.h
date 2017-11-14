/* TAD referente a B-tree para ser armazenadas em arquivos */

#ifndef BTREE_H_
#define BTREE_H_

#define ORDEM 5
#define TRUE 1
#define FALSE 0
#define PROMOTION 2
#define NO_PROMOTION 3
#define ERRO 4

typedef struct cabecalho_btree {
  int noRaiz;
  int estaAtualizada;
} CABECALHO_BTREE;

typedef struct chave {
  int id;
  int offset;
} CHAVE;

typedef struct pagina {
  int contadorPagina; // Guarda o numero da pagina na qual estamos.
  int numeroChaves; // Guarda o numero de chaves do no.
  CHAVE chaves[ORDEM-1]; // Vetor de chaves do no.
  int filhos[ORDEM]; // Armazena os enderecos referentes aos filhos.
  int isFolha; // Bool que identifica um no folha.
} PAGINA;

/* Esta pagina sera somente utilizada no caso de um split, no qual precisamos
 * temporariamente de um noh que comporte uma chave a mais */
typedef struct paginaSplit {
  int contadorPagina; // Guarda o numero da pagina na qual estamos.
  int numeroChaves; // Guarda o numero de chaves do no.
  CHAVE chaves[ORDEM]; // Vetor de chaves do no.
  int filhos[ORDEM+1]; // Armazena os enderecos referentes aos filhos.
  int isFolha; // Bool que identifica um no folha.
} PAGINA_SPLIT;


void criaBT(FILE *arq);
int inserirBT(FILE *arq, int offset, CHAVE chave, int chavePromovida, int direitoChavePromovida);
int buscaBT(FILE *arq, int RRN, int chave, int RRN_encontrado, int pos_encontrada);
void split(FILE *arq, int i_key, int i_RRN, PAGINA p, int promo_key, int promo_r_child, PAGINA newp);
void ler_criacao_btree(FILE *arq);
int buscaRaiz(FILE *arq);


#endif //BTREE_H_
