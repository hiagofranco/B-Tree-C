/* TAD referente a registros de tamanho variavel */

#ifndef REGISTRO_H_
#define REGISTRO_H_

#define tamTitulo 30
#define tamGenero 20
#define tamId 10

typedef struct registro {
  int id;
  char titulo[tamTitulo];
  char genero[tamGenero];
} REGISTRO;

typedef struct cabecalho_dados
{
    long int byteoffset_ultimo;
}CABECALHO_DADOS;

int tam_reg(REGISTRO r, char *buffer);
char *parser(char *buffer, int *pos);
void inserir_arquivo(FILE *arq,REGISTRO r);
void inserir_registro(REGISTRO *r);
void inserir_cabecalho(FILE *arq);
void buscar_registro(FILE *arq,REGISTRO r,int byteOffset);
void remocao_registro(FILE *arq,REGISTRO r,int byteOffset);
char byte_offset_ultimo_inserido(FILE *arq);
void ler_ultimo_registro(FILE *arq,REGISTRO r);
void imprimirArquivoDados(FILE *arq);
#endif //REGISTRO_H_
