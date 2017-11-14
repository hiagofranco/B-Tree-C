#include"Log.h"

void log_insercao(FILE *arq,int id,char *titulo,char *genero)
{
    arq = fopen("log_Hiago.txt","a");
    if(arq==NULL)
    {
        printf("Erro ao abrir o arquivo");
        exit(1);
    }
    else
    fprintf(arq,"Execucao de operacao de INSERCAO de <%d>, <%s>,<%s>\n",id,titulo,genero);
    fclose(arq);
}
void log_indice(FILE *arq)
{
    arq = fopen("log_Hiago.txt","a");
    if(arq==NULL)
    {
        printf("Erro ao abrir o arquivo");
        exit(1);
    }
    else
    fprintf(arq,"Execucao da criacao do arquivo de indice <arvore.idx> com base no arquivo de dados <dados.dad>\n");
    fclose(arq);
}
void log_busca(FILE *arq,int id,char *titulo,char *genero,int offset)
{
    if(arq==NULL)
    {
        printf("Erro ao abrir o arquivo");
        exit(1);
    }
    fprintf(arq,"Execucao de operacao de PESQUISA de <%d>\n",id);
    fclose(arq);
}
void log_remocao(FILE *arq,int id)
{
    if(arq==NULL)
    {
        printf("Erro ao abrir o arquivo");
        exit(1);
    }
    fprintf(arq,"Execucao de operacao de REMOCAO de <%d>\n",id);
    fclose(arq);
}
void log_mostrarArvore(FILE *arq,int id)
{
    if(arq==NULL)
    {
        printf("Erro ao abrir o arquivo");
        exit(1);
    }
    fprintf(arq,"Execucao de operacao de mostar arvore-B gerada\n");
    fclose(arq);
}
