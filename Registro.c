#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "B-Tree.h"
#include "Registro.h"
#include <ctype.h>

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

    //Insere o novo cabecalho no comeco do arquivo
    fseek(arq,0,0);
    fwrite(&size_byteoffset_novo,sizeof(size_byteoffset_novo),1,arq);
}

void inserir_arquivo(FILE *arq,REGISTRO r)
{
    CABECALHO_DADOS c;

    char buffer[1000];
    char size;

    size = tam_reg(r, buffer);
    fseek(arq, 0, SEEK_END);

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
}
void inserir_registro(REGISTRO *r)
{
    char id[100];
    int i,tamanho,controle;
    do
    {
        controle=0;
        printf("ID:");
        fgets(id,sizeof(id),stdin);
        id[strcspn(id, "\n")] = '\0';
        tamanho = strlen(id);
        for(i=0;i<tamanho;i++)
        {
            if(isdigit(id[i])==0)
            {
                controle=1;
            }
        }
        r->id = atoi(id);

        printf("Titulo:");
        fgets(r->titulo, tamTitulo, stdin);
        r->titulo[strcspn(r->titulo, "\n")] = '\0';     /* strcspn procura a primeira ocorrencia  do segundo parametro no primeiro
                                                        e retorna a posi��o do vetor na qual ocorre, assim podemos tirar o \n
                                                        que aparece quando apertamos enter e o substituimos por \0  */
        tamanho = strlen(r->titulo);
        for(i=0;i<tamanho;i++)
        {
            if(r->titulo[i]!=' ')
            {
                if(isalpha(r->titulo[i])==0)
                {
                    controle=1;
                }
            }
        }

        printf("Genero:");
        fgets(r->genero, tamGenero, stdin);
        r->genero[strcspn(r->genero, "\n")] = '\0';

        tamanho = strlen(r->genero);
        for(i=0;i<tamanho;i++)
        {
            if(r->genero[i]!=' ')
            {
                if(isalpha(r->genero[i])==0)
                {
                    controle=1;
                }
            }
        }
        if(controle==1)
        {
            printf("Ocorreu um erro,caracteres invalidos inseridos\n");
            printf("\n Por favor, digite os dados a serem inseridos na seguinte ordem e separados por \"\\n\":\n"
            "Numero inteiro com ID da musica.\n"
            "Titulo da musica.\n"
            "Genero da musica.\n"
            "> ");
        }
    } while(controle==1);
}

void imprimirArquivoDados(FILE *arq){
    CABECALHO_DADOS c;
    REGISTRO r;
    char size, buffer[1000];
    fread(&c, sizeof(CABECALHO_DADOS), 1, arq);

    printf("\nByte Offset ultimo registro = %ld  ftell = %ld\n\n", c.byteoffset_ultimo, ftell(arq));

    int j = 1;
    while (fread(&size, sizeof(size), 1, arq))
    {
        printf("Byte Offset do %d registro = %ld\n", j, ftell(arq) - 1);
        fread(buffer, size, 1, arq);
        j++;

        int pos = 0;
        sscanf(parser(buffer, &pos), "%d", &r.id);
        strcpy(r.titulo, parser(buffer, &pos));
        strcpy(r.genero, parser(buffer, &pos));
        printf("size: %d  ID: %d Titulo: %s Genero: %s\n\n", (int)size, r.id, r.titulo, r.genero);
    }
}

void lerRegistro(FILE *arq, REGISTRO *r, long int offset){
    char buffer[1000], size;

    arq = fopen("dados.dad","rb+");
    if(!arq){
        printf("Erro na abertura do arquivo de dados (dados.dad)");
        exit(1);
    }
    fseek(arq, offset, SEEK_SET);

    fread(&size, sizeof(size), 1, arq);
    fread(buffer, size, 1, arq);
    int pos = 0;
    sscanf(parser(buffer, &pos), "%d", &r->id);
    strcpy(r->titulo, parser(buffer, &pos));
    strcpy(r->genero, parser(buffer, &pos));
}
