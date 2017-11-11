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

void inserir_cabecalho(FILE *arq)
{
    fseek(arq,0,SEEK_END);

    //Me da o byteoffset do fim do arquivo para ser o novo cabecalho
    long int size_byteoffset_novo = ftell(arq);
    long int size_byteoffset_antigo;

    //Peguei de teste para ver se o cabecalho antigo foi inserido certo
    fseek(arq,0,0);
    fread(&size_byteoffset_antigo,sizeof(size_byteoffset_antigo),1,arq);
    printf("Antigo: %ld\n",size_byteoffset_antigo);

    //Novo cabecalho a ser introduzido no comeco do arquivo
    printf("Novo %ld\n",size_byteoffset_novo);

    //Insere o novo cabecalho no comeco do arquivo
    fseek(arq,0,0);
    fwrite(&size_byteoffset_novo,sizeof(size_byteoffset_novo),1,arq);
}

void inserir_arquivo(FILE *arq,REGISTRO r)
{
    CABECALHO c;

    char buffer[1000];
    char size;

    size = tam_reg(r, buffer);
    fseek(arq, SEEK_END, 0);
    printf("ftell = %ld\n", ftell(arq));

    if(ftell(arq)==0)
    {
        c.byteoffset_ultimo = sizeof(c);
        fwrite(&c.byteoffset_ultimo,sizeof(c),1,arq);
    }
    else
    {
        fclose(arq);

        arq = fopen("dados.dad","rb+");
        if(!arq)
        {
            printf("Erro ao abrir o arquivo de dados! (dados.dad)\n");
            exit(1);
        }
        inserir_cabecalho(arq);
    }
    //Volta para o final para inserir os dados da forma pedida
    fseek(arq,0,SEEK_END);

    fwrite(&size, sizeof(size), 1, arq);
    fwrite(buffer, size, 1, arq);

    //imprime o que está no buffer (o que vai ser escrito no arquivo, o registro) e o tamanho dele
    printf("\nBuffer(%s) \nsize %d  \n\n", buffer, (int)size);

    long int i;
    fseek(arq,0,0);
    fread(&i, sizeof(c), 1, arq);

    printf("\ni = %ld  ftell = %ld\n", i, ftell(arq));

    while (fread(&size, sizeof(size), 1, arq))
    {
        printf("ftell = %ld\n", ftell(arq));
        fread(buffer, size, 1, arq);
        printf("ftell = %ld\n", ftell(arq));
        int pos = 0;
        sscanf(parser(buffer, &pos), "%d", &r.id);
        strcpy(r.titulo, parser(buffer, &pos));
        strcpy(r.genero, parser(buffer, &pos));
        printf("size: %d  ID: %d Titulo: %s Genero: %s\n", (int)size, r.id, r.titulo, r.genero);
    }
    /*printando o que tem no arquivo pra ver se a inserção está correta*/
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
char byte_offset_ultimo_inserido(FILE *arq)
{
    fseek(arq,0,SEEK_SET);
    char byteOffset;
    fread(&byteOffset,sizeof(byteOffset),1,arq);
    return byteOffset;
}
void remocao_registro(FILE *arq,REGISTRO r,int byteOffset)
{
    //tem que abrir o arquivo com a+
    //exemplo na main
    char flag = '*';
    fseek(arq,byteOffset+1,0);
    fwrite(&flag, sizeof(flag),1, arq);
}
