#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Registro.h"


int tam_reg(REGISTRO r, char *buffer)
{
     sprintf(buffer, "%d|%s|%s|",r.id,r.titulo,r.genero);
     return strlen(buffer);
}

char *parser(char *buffer, int *pos)
{
    int posi = *pos;

    while(buffer[*pos]!='|')
        (*pos)++;
    buffer[*pos] = '\0';
    (*pos)++;
    return &buffer[posi];
}

void inserir_arquivo(FILE *arq,REGISTRO r)
{
    char buffer[1000];
    char size;

    size = tam_reg(r, buffer);
    fwrite(&size, sizeof(size), 1, arq);
    fwrite(buffer, size, 1, arq);

    //imprime o que está no buffer (o que vai ser escrito no arquivo, o registro) e o tamanho dele
    printf("\nBuffer(%s) \nsize %d  \n\n", buffer, (int)size);

    /*fseek(arq,0,0);
    while (fread(&size, sizeof(size), 1, arq))
    {
        fread(buffer, size, 1, arq);
        int pos = 0;
        sscanf(parser(buffer, &pos), "%d", &r.id);
        strcpy(r.titulo, parser(buffer, &pos));
        strcpy(r.genero, parser(buffer, &pos));
        printf("ID: %d Titulo: %s Genero: %s\n", r.id, r.titulo, r.genero);
    }
    //printando o que tem no arquivo pra ver se a inserção está correta*/
}
void inserir_registro(REGISTRO *r)
{
    scanf("%d", &r->id);
    fflush(stdin);
    fgets(r->titulo, tamTitulo, stdin);
    r->titulo[strcspn(r->titulo, "\n")] = '\0';     /* strcspn procura a primeira ocorrencia  do segundo parametro no primeiro
                                                    e retorna a posição do vetor na qual ocorre, assim podemos tirar o \n
                                                        que aparece quando apertamos enter e o substituimos por \0  */
    fgets(r->genero, tamGenero, stdin);
    r->genero[strcspn(r->genero, "\n")] = '\0';
    return;
}
//busca um registro de acordo com o deslocamento ao ser realizado no arquivo
void buscar_registro(FILE *arq,REGISTRO r,int byteOffset)
{
    char size;
    char buffer[100];
    int pos = 0;

    fseek(arq,byteOffset,0);

    fread(&size, sizeof(size), 1, arq);
    fread(buffer, size, 1, arq);

    sscanf(parser(buffer, &pos), "%d", &r.id);
    strcpy(r.titulo, parser(buffer, &pos));
    strcpy(r.genero, parser(buffer, &pos));
    printf("ID: %d Titulo: %s Genero: %s\n", r.id, r.titulo, r.genero);
}
void remocao_registro(FILE *arq,REGISTRO r,int byteOffset)
{
    //tem que abrir o arquivo com a+
    //exemplo na main
    char flag = '*';
    fseek(arq,byteOffset,0);
    fwrite(&flag, sizeof(flag),1, arq);
}

