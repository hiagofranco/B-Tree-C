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

typedef struct cabecalho
{
    int num_registros;
    int isAtualizado;
}CABECALHO;

int tam_reg(REGISTRO r, char *buffer);
void inserir_arquivo(FILE *arq,REGISTRO r);
void inserir_registro(REGISTRO *r);
void buscar_registro(FILE *arq,REGISTRO r,int byteOffset);
void remocao_registro(FILE *arq,REGISTRO r,int byteOffset);
#endif //REGISTRO_H_
