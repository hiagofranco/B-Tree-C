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

void inserir_arquivo(FILE *arq,REGISTRO r,char *buffer)
{
    char size;
    int pos;

       arq = fopen("dados.dad","ab");
        if(arq==NULL)
        {
            printf("Arquivo Inexistente");
            exit(-1);
        }
        size = tam_reg(r, buffer);
        fwrite(&size, sizeof(size), 1, arq);
        fwrite(buffer, size, 1, arq);

        fseek(arq, 0, 0);

    while (fread(&size, sizeof(size), 1, arq))
    {
        fread(buffer, size, 1, arq);
        pos = 0;
        sscanf(parser(buffer, &pos), "%d", &r.id);
        strcpy(r.titulo, parser(buffer, &pos));
        strcpy(r.genero, parser(buffer, &pos));
        printf("%d %s %s\n", r.id, r.titulo, r.genero);
    }
}
void inserir_registro(REGISTRO *r,char titulo[tamTitulo],char genero[tamGenero],int id)
{
    strcpy(r->titulo,titulo);
    strcpy(r->genero,genero);
    r->id = id;
}

