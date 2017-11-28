#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "B-Tree.h"
#include "Registro.h"

#define TRUE 1
#define FALSE 0

int main()
{
  printf("Seja bem vindo!\n");

  //Variaveris de controle do menu.
  int end = FALSE, option;

  //Variavel referente a manipulacao dos arquivos.
  FILE *arq;
  FILE *index;

  //Variaveis utilizadas para leitura de dados.
  REGISTRO r;
  //CHAVE chave;
  CABECALHO_DADOS cabecalhoDados;

  //Loop principal do programa.
  while(!end) {
    //Impressao do menu.
    printf("\nSelecione uma das opcoes abaixo:\n"
    "1. Criar indice.\n"
    "2. Inserir Musica.\n"
    "3. Pesquisar Musica por ID.\n"
    "4. Remover Musica por ID.\n"
    //"5. Mostrar Arvore-B.\n"
    "5. Exibir Arquivo de dados.\n"
    "6. Fechar o programa.\n"
    "> ");
    scanf("%d", &option);

    switch(option)
    {
      /*Funcionalidade 1 - Cria um indice a partir de um arquivo de dados*/
      case 1:
        arq = fopen("dados.dad", "rb+"); //Abre o arquivo no modo append, para ser que os
                                        //novos dados sejam escritos no final do arquivo.
        if(!arq)
        {
            printf("Erro ao abrir o arquivo de dados! (dados.dad)\n");
            exit(1);
        }
        fread(&cabecalhoDados, sizeof(CABECALHO_DADOS), 1, arq);

        index = fopen("arvore.idx", "rb+");
        if(!index)
        {
            index = fopen("arvore.idx", "wb+");
            fclose(index);
            index = fopen("arvore.idx", "rb+");
        }
        criaBT(index);

        fclose(index);

        fopen("arvore.idx", "rb+");
        ler_criacao_btree(index);


        fclose(index);
        fclose(arq);

        break;

      /*Funcionalidade 2 - Insercao	de	novas	m�sicas	no	arquivo	de	dados	e	no	�ndice*/
      case 2:

        arq = fopen("dados.dad", "ab+"); //Abre o arquivo no modo append, para ser que os
        //novos dados sejam escritos no final do arquivo.
        if(!arq)
        {
            printf("Erro ao abrir o arquivo de dados! (dados.dad)\n");
            exit(1);
        }

        printf("\nDigite os dados a serem inseridos na seguinte ordem e separados por \"\\n\":\n"
        "Numero inteiro com ID da musica.\n"
        "Titulo da musica.\n"
        "Genero da musica.\n"
        "> ");
        inserir_registro(&r);
        inserir_arquivo(arq,r);

        fclose(arq);

        break;
      /* Funcionalidade 3 - Pesquisa (busca)	por	Id	da	m�sica */
      case 3:

        //TESTE PARA LER O ULTIMO REGISTRO
        arq = fopen("dados.dad","rb+");
        if(!arq)
        {
            printf("Erro ao abrir o arquivo de dados! (dados.dad)\n");
            exit(1);
        }

       ler_ultimo_registro(arq, r);

        break;
      /* Funcionalidade 4 - Remo��o	de m�sica	a	partir	do	Id */
      case 4:
        break;
      /* Funcionalidade 5 - Mostrar	 �rvore-B */
      case 5:
          arq = fopen("dados.dad", "rb+");
          imprimirArquivoDados(arq);
        break;
      /* Sair do programa */
      case 6:
        printf("\nTchau! =)\n");
        end = TRUE;
        break;
      /* Entrada default */
      default:
        printf("\nComando invalido! Tente novamente.\n");
        break;
    }
  }
  return 0;
}
