/* TAD referente a registros de tamanho variavel */

#ifndef REGISTRO_H_
#define REGISTOR_H_

#define tamTitulo 30
#define tamGenero 20

typedef struct registro {
  int id;
  char titulo[tamTitulo];
  char genero[tamGenero];
} REGISTRO;

int tamanhoRegistro(REGISTRO r, char *buffer);
int 

#endif //REGISTRO_H_
