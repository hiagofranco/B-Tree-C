/* TAD referente a registros de tamanho variavel */

#ifndef REGISTRO_H_
#define REGISTOR_H_

#define tamTitulo 30
#define tamGenero 20
#define tamId 10

typedef struct registro {
  int id;
  char titulo[tamTitulo];
  char genero[tamGenero];
} REGISTRO;

int tam_reg(REGISTRO r, char *buffer);
void inserir_arquivo(FILE *arq,REGISTRO r,char *buffer);
void inserir_registro(REGISTRO *r,char titulo[tamTitulo],char genero[tamGenero],int id);
#endif //REGISTRO_H_
