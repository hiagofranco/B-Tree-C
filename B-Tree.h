/* TAD referente a B-tree para ser armazenadas em arquivos */

#ifndef BTREE_H_
#define BTREE_H_

#define ORDEM 5

typedef struct pagina
{
  int numeroChaves; // Guarda o numero de chaves do no.
  int chaves[ORDEM-1]; // Vetor de chaves do no.
  int filhos[ORDEM]; //armazena o RRN dos filhos.
  int isFolha; // bool que identifica um no folha.
} PAGINA;

void inserirBT();
int busca(int RRN, int chave, int RRN_encontrado, int pos_encontrada);
void removerBT();
void printBT();

#endif //BTREE_H_
