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
    long int byteoffset_ultimo;
}CABECALHO;

int tam_reg(REGISTRO r, char *buffer);
void inserir_arquivo(FILE *arq,REGISTRO r);
void inserir_registro(REGISTRO *r);
void inserir_cabecalho(FILE *arq);
void buscar_registro(FILE *arq,REGISTRO r,int byteOffset);
void remocao_registro(FILE *arq,REGISTRO r,int byteOffset);
char byte_offset_ultimo_inserido(FILE *arq);
void ler_ultimo_registro(FILE *arq,REGISTRO r);
#endif //REGISTRO_H_
