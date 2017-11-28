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
  FILE *arq; // Arquivo de dados
  FILE *index; //Arquivo da b-Tree

  //Variaveis utilizadas para leitura de dados.
  REGISTRO r;
  //CHAVE chave;
  //CABECALHO_DADOS cabecalhoDados;

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
        arq = fopen("dados.dad", "rb");
        if(!arq)
        {
            printf("Erro ao abrir o arquivo de dados na Funcionalidade 1! (dados.dad)\n");
            break;
        }

        /* Inicio do algoritmo de driver */
          criaBT(index); //Cria a raiz e o cabecalho
          index = fopen("arvore.idx", "rb+");
          // Pula o cabecalho e vai para o primeiro dado.
          fseek(arq, sizeof(CABECALHO_DADOS), SEEK_SET);

          //Tras o cabecalho da b-tree para a memoria.
          CABECALHO_BTREE cabecalhoTree;
          fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

          // Le o tamanho do registro.
          char size;
          fread(&size, sizeof(size), 1, arq);

          CHAVE chaveInserida; //Varaivel de chave a ser inserida

          /* Pego o endereco e subtraio o valor da varivel de tamanho, ja que
          esta valor nao entra do calculo do registro */
          chaveInserida.offset = ftell(arq) - sizeof(size);

          // Criar um buffer para armaezenar o registro.
          char buffer[1000];
          fread(buffer, size, 1, arq);

          // Divide o registro ate o divisor de campos, e aramezena o id.
          int pos = 0;
          sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);

          CHAVE chavePromovida;
          chavePromovida.id = -1;
          chavePromovida.offset = -1;
          printf("\nnao inseriu");

          // Insere o no raiz;
          inserirBT(index, 0, chaveInserida, chavePromovida, -1, &(cabecalhoTree.contadorDePaginas));

          printf("\ninseriu primeiro");
          while(fread(&size, sizeof(size), 1, arq)) {

            // Pega o proximo registro para inserir na b-tree;
            chaveInserida.offset = ftell(arq) - sizeof(size);
            fread(buffer, size, 1, arq);
            pos = 0;
            sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);
            printf("\nInserindo ID %d", chaveInserida.id);

            //Coloca os valores em default.
            chavePromovida.id = -1;
            chavePromovida.offset = -1;
            printf("\ncontadorDePaginas = %d", cabecalhoTree.contadorDePaginas);

            int promo_r_child = -1;
            int root = buscaRaiz(index);
            if(inserirBT(index, root, chaveInserida, chavePromovida, promo_r_child, &(cabecalhoTree.contadorDePaginas)) == PROMOTION) {
              PAGINA newRoot;

              cabecalhoTree.contadorDePaginas++;
              newRoot.RRNDaPagina = cabecalhoTree.contadorDePaginas;

              int i;
              for(i = 0; i < ORDEM-1; i++) {
                newRoot.chaves[i].id = -1;
                newRoot.chaves[i].offset = -1;
              }
              for(i = 0; i < ORDEM; i++) newRoot.filhos[i] = -1;

              newRoot.numeroChaves = 1;
              newRoot.chaves[0] = chavePromovida;

              newRoot.filhos[0] = root;
              newRoot.filhos[1] = promo_r_child;

              cabecalhoTree.noRaiz = newRoot.RRNDaPagina;
            }
          }
          fseek(index, 0, 0);
          fwrite(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);
          fclose(index);

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
          ler_btree(index);
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
