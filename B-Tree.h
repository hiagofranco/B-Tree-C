/* TAD referente a B-tree para ser armazenadas em arquivos */

#ifndef BTREE_H_
#define BTREE_H_

#define ORDEM 5
#define TRUE 1
#define FALSE 0
#define PROMOTION 2
#define NO_PROMOTION 3
#define ERROR 4

typedef struct cabecalho {
  int noRaiz;
  int estaAtualizada;
  int contadorDePaginas;
} CABECALHO_BTREE;

typedef struct chave {
  int id;
  long int offset;
} CHAVE;

typedef struct pagina {
  int RRNDaPagina; // Guarda o numero da pagina na qual estamos.
  int numeroChaves; // Guarda o numero de chaves do no.
  CHAVE chaves[ORDEM-1]; // Vetor de chaves do no.
  int filhos[ORDEM]; // Armazena os enderecos referentes aos filhos.
} PAGINA;

/* Esta pagina sera somente utilizada no caso de um split, no qual precisamos
 * temporariamente de um noh que comporte uma chave a mais */
typedef struct paginaSplit {
  CHAVE chaves[ORDEM]; // Vetor de chaves do no.
  int filhos[ORDEM+1]; // Armazena os enderecos referentes aos filhos.
} PAGINA_SPLIT;

void criaBT();
int inserirBT(FILE *arq, FILE *logTxt, int offset, CHAVE *chave, CHAVE *chavePromovida, int *direitoChavePromovida, int *contadorDePaginas);
int buscaBT(FILE *arq, int offset, int id, long int *offset_encontrado);
void split(FILE *arq, int i_key, int i_offset, PAGINA *p, CHAVE *promo_key, int *promo_r_child, PAGINA *newP, int *contadorDePaginas, int RRNPaginaSplitada);
void ler_criacao_btree(FILE *index);
int buscaRaiz(FILE *arq);
void ler_btree(FILE *arq);
void desatualizarBTree(FILE *index, CABECALHO_BTREE cabecalho);
void atualizarBTree(FILE *index, CABECALHO_BTREE cabecalho);
void exibirBT(FILE *index, FILE *logTxt, int root);

#endif //BTREE_H_
