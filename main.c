#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Registro.h"
#include "B-Tree.h"
#include "Log.h"

#define TRUE 1
#define FALSE 0

/* Funcao referente ao fflush do Windows. Foi usada aqui para que o programa
 * rode tanto no Linux quanto no Windows. Essa funcao foi retirada desta pagina:
 * https://stackoverflow.com/questions/17318886/fflush-is-not-working-in-linux */
void clean_stdin() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

/* Inicio da main */
int main() {
  printf("Seja bem vindo!\n");

  //Variaveris de controle do menu.
  int end = FALSE, option;
  int invalido = -1;

  //Variavel referente a manipulacao dos arquivos.
  FILE *arq; // Arquivo de dados
  FILE *index; //Arquivo da b-Tree
  FILE *logTxt; // Arquivo de log, saida do programa.
  logTxt = fopen("log_HMoreira.txt", "w");
  if(!logTxt) {
    printf("Erro ao abrir o arquivo de log! (log_HMoreira.txt)\n");
    exit(1);
  }

  //Variaveis utilizadas para leitura de dados.
  REGISTRO r;
  CHAVE chaveInserida; //Varaivel de chave a ser inserida
  CABECALHO_BTREE cabecalhoTree;

  //Loop principal do programa.
  while(!end) {
    //Impressao do menu.
    printf("\nSelecione uma das opcoes abaixo:\n"
    "1. Criar indice.\n"
    "2. Inserir Musica.\n"
    "3. Pesquisar Musica por ID.\n"
    "4. Exibir Arquivo da B-Tree.\n"
    "5. Exibir Arquivo de dados.\n"
    "6. Fechar o programa.\n"
    "> ");
    scanf("%d", &option);
    clean_stdin();

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
          if(!index) {
            printf("Erro ao abrir o arquivo de index na Funcionalidade 1! (arvore.idx)\n");
            break;
          }

          // Grava que a funcionalidade 1 esta sendo executada no arquivo de log.
          log_indice(logTxt);

          // Pula o cabecalho do arquivo de dados e vai para o primeiro dado.
          fseek(arq, sizeof(CABECALHO_DADOS), SEEK_SET);

          //Tras o cabecalho da b-tree para a memoria.
          fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

          // Le o tamanho do registro.
          char size;
          fread(&size, sizeof(size), 1, arq);

          /* Pego o endereco e subtraio o valor da varivel de tamanho, ja que
          esta valor nao entra do calculo do registro */
          chaveInserida.offset = ftell(arq) - sizeof(size);

          // Criar um buffer para armaezenar o registro.
          char buffer[1000];
          fread(buffer, size, 1, arq);

          // Divide o registro ate o divisor de campos, e aramezena o id, titulo e genero.
          int pos = 0;
          sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);
          r.id = chaveInserida.id; // Usada para guardar a chave para que possamos gravra no log.
          strcpy(r.titulo, parser(buffer, &pos));
          strcpy(r.genero, parser(buffer, &pos));

          // Cria uma chave para ser utilizada como parametro de retorno da recursao.
          CHAVE chavePromovida;
          chavePromovida.id = -1;
          chavePromovida.offset = -1;

          // Insere o no raiz;
          log_insercao(logTxt, &chaveInserida, 0, NULL, &r);
          int return_log =  inserirBT(index, logTxt, 0, &chaveInserida, &chavePromovida, &invalido, &(cabecalhoTree.contadorDePaginas));
          if(return_log == ERROR) log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
          else log_insercao(logTxt, NULL, 3, NULL, &r);

          while(fread(&size, sizeof(size), 1, arq)) {

            // Pega o proximo registro para inserir na b-tree;
            chaveInserida.offset = (int)ftell(arq) - sizeof(size);
            fread(buffer, size, 1, arq);
            pos = 0;
            sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);
            r.id = chaveInserida.id; // Usada para guardar a chave para que possamos gravra no log.
            strcpy(r.titulo, parser(buffer, &pos));
            strcpy(r.genero, parser(buffer, &pos));

            //Coloca os valores em default.
            chavePromovida.id = -1;
            chavePromovida.offset = -1;

            int promo_r_child = -1;
            int root = cabecalhoTree.noRaiz;

            return_log = inserirBT(index, logTxt, root, &chaveInserida, &chavePromovida, &promo_r_child, &(cabecalhoTree.contadorDePaginas));

            if(return_log == ERROR) log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
            else log_insercao(logTxt, NULL, 3, NULL, &r);

            if(return_log == PROMOTION) {

              PAGINA newRoot;

              cabecalhoTree.contadorDePaginas++;
              newRoot.RRNDaPagina = cabecalhoTree.contadorDePaginas - 1;

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
              fseek(index, sizeof(CABECALHO_BTREE) + (newRoot.RRNDaPagina)*sizeof(PAGINA), SEEK_SET);
              fwrite(&newRoot, sizeof(PAGINA), 1, index);
            }
          }
          fseek(index, 0, 0);
          fwrite(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);
          fclose(index);
          fclose(arq);

        break;

      /*Funcionalidade 2 - Insercao	de	novas	musicas	no	arquivo	de	dados	e	no	indice*/
      case 2:
        arq = fopen("dados.dad", "ab+"); //Abre o arquivo no modo append, para ser que os
        //novos dados sejam escritos no final do arquivo.
        if(!arq) {
            printf("Erro ao abrir o arquivo de dados! (dados.dad)\n");
            exit(1);
        }

        printf("\nDigite os dados a serem inseridos:\n");
        inserir_registro(&r);
        inserir_arquivo(arq,r);

        /* Algoritmo driver para a insercao de apenas uma chave */
        index = fopen("arvore.idx", "rb+");
        if(!index) {
          printf("Arvore ainda nao criada! Criando uma B-Tree...\n");
          criaBT(index);
          // Agora o arquivo ja foi criado.
          index = fopen("arvore.idx", "rb+");
          if(!index) {
            printf("Erro ao abrir o arquivo de indice! (arvore.idx)\n");
            exit(1);
          }
        }

        // Vai para o comeco do arquivo.
        fseek(arq, 0, SEEK_SET);

        // Busca informacao do ultimo registro do cabecalho.
        ler_ultimo_registro(arq, &r, &chaveInserida);

        //Tras o cabecalho da b-tree para a memoria.
        fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

        // Cria uma chave para ser utilizada como parametro de retorno da recursao.
        CHAVE chavePromovida;
        chavePromovida.id = -1;
        chavePromovida.offset = -1;

        // Insere o no raiz;
        log_insercao(logTxt, &chaveInserida, 0, NULL, &r);
        int return_log =  inserirBT(index, logTxt, 0, &chaveInserida, &chavePromovida, &invalido, &(cabecalhoTree.contadorDePaginas));
        if(return_log == ERROR) log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
        else log_insercao(logTxt, NULL, 3, NULL, &r);

        while(fread(&size, sizeof(size), 1, arq)) {

          // Pega o proximo registro para inserir na b-tree;
          chaveInserida.offset = (int)ftell(arq) - sizeof(size);
          fread(buffer, size, 1, arq);
          pos = 0;
          sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);
          r.id = chaveInserida.id; // Usada para guardar a chave para que possamos gravra no log.
          strcpy(r.titulo, parser(buffer, &pos));
          strcpy(r.genero, parser(buffer, &pos));

          //Coloca os valores em default.
          chavePromovida.id = -1;
          chavePromovida.offset = -1;

          int promo_r_child = -1;
          int root = cabecalhoTree.noRaiz;

          return_log = inserirBT(index, logTxt, root, &chaveInserida, &chavePromovida, &promo_r_child, &(cabecalhoTree.contadorDePaginas));

          if(return_log == ERROR) log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
          else log_insercao(logTxt, NULL, 3, NULL, &r);

          if(return_log == PROMOTION) {

            PAGINA newRoot;

            cabecalhoTree.contadorDePaginas++;
            newRoot.RRNDaPagina = cabecalhoTree.contadorDePaginas - 1;

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
            fseek(index, sizeof(CABECALHO_BTREE) + (newRoot.RRNDaPagina)*sizeof(PAGINA), SEEK_SET);
            fwrite(&newRoot, sizeof(PAGINA), 1, index);
          }
        }
        fseek(index, 0, 0);
        fwrite(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

        fclose(arq);
        fclose(index);

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
