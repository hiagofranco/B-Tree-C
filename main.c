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
  CHAVE chaveInserida; //Variavel de chave a ser inserida
  CHAVE chavePromovida;
  CABECALHO_BTREE cabecalhoTree;
  CABECALHO_DADOS cabecalhoDados;
  PAGINA newRoot;
  int root, promo_r_child, return_log; //variaveis necessarias para a criacao da B-Tree
  int i;  //Variavel auxiliar para loops
  //Variaveis para a leitura de novos registros
  char buffer[1000], size;
  int pos;
  //Variaveis para a funcionalidade de busca
  int idProcurado;
  long int offsetProcurado;

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
    "7. Mostrar Arvore-B.\n"
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
            printf("Erro ao abrir o arquivo de dados (dados.dad) na Funcionalidade 1! \nTalvez o arquivo de dados ainda nao exista, insira algumas musicas com a Funcionalidade 2 e tente novamente\n");
            break;
        }

        /* Inicio do algoritmo de driver */
          criaBT(index); //Cria a raiz e o cabecalho
          index = fopen("arvore.idx", "rb+");
          if(!index) {
            printf("Erro ao abrir o arquivo de indice (arvore.idx) na Funcionalidade 1! \n");
            break;
          }

          // Grava que a funcionalidade 1 esta sendo executada no arquivo de log.
          log_indice(logTxt);

          // Pula o cabecalho do arquivo de dados e vai para o primeiro dado.
          fseek(arq, sizeof(CABECALHO_DADOS), SEEK_SET);

          //Tras o cabecalho da b-tree para a memoria.
          fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

          // Le o tamanho do registro.
          fread(&size, sizeof(size), 1, arq);

          /* Pego o endereco e subtraio o valor da varivel de tamanho, ja que
          esta valor nao entra do calculo do registro */
          chaveInserida.offset = ftell(arq) - sizeof(size);

          // Criar um buffer para armaezenar o registro.
          fread(buffer, size, 1, arq);

          // Divide o registro ate o divisor de campos, e aramezena o id, titulo e genero.
          pos = 0;
          sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);
          r.id = chaveInserida.id; // Usada para guardar a chave para que possamos gravra no log.
          strcpy(r.titulo, parser(buffer, &pos));
          strcpy(r.genero, parser(buffer, &pos));

          // Cria uma chave para ser utilizada como parametro de retorno da recursao.
          chavePromovida.id = -1;
          chavePromovida.offset = -1;

          desatualizarBTree(index, cabecalhoTree);

          // Insere o no raiz;
          log_insercao(logTxt, &chaveInserida, 0, NULL, &r);
          return_log =  inserirBT(index, logTxt, 0, &chaveInserida, &chavePromovida, &invalido, &(cabecalhoTree.contadorDePaginas));
          if(return_log == ERROR) {
                log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
          }

          else {
                log_insercao(logTxt, NULL, 3, NULL, &r);
          }

          atualizarBTree(index, cabecalhoTree);

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
            promo_r_child = -1;

            root = cabecalhoTree.noRaiz;

            log_insercao(logTxt, &chaveInserida, 0, NULL, &r);

            desatualizarBTree(index, cabecalhoTree);

            return_log = inserirBT(index, logTxt, root, &chaveInserida, &chavePromovida, &promo_r_child, &(cabecalhoTree.contadorDePaginas));

            if(return_log == ERROR) log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
            else log_insercao(logTxt, NULL, 3, NULL, &r);

            if(return_log == PROMOTION) {

              cabecalhoTree.contadorDePaginas++;
              newRoot.RRNDaPagina = cabecalhoTree.contadorDePaginas - 1;

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

            atualizarBTree(index, cabecalhoTree);
          }

          fseek(index, 0, 0);
          fwrite(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

          fclose(index);
          fclose(arq);

        break;

      /*Funcionalidade 2 - Insercao	de	novas	musicas	no	arquivo	de	dados	e	no	indice*/
      case 2:

        arq = fopen("dados.dad", "ab+"); /*Abre o arquivo no modo append, para ser que os novos dados
                                          sejam escritos no final do arquivo. */

        if(!arq) {
            printf("Erro ao abrir o arquivo de dados! (dados.dad)\n");
            exit(1);
        }

        printf("\nDigite os dados a serem inseridos:\n");
        inserir_registro(&r);
        inserir_arquivo(arq,r);

        index = fopen("arvore.idx", "rb+");

        //Nesse caso seguinte temos o arquivo de dados, mas nao o da B-Tree, entao temos que cria-la integralmente
        //Essa condicao sera verdadeira na primeira insercao e quando o arquivo de indice for deletado, mas o de dados for conservado
        if(!index) {
          printf("Arvore ainda nao criada! Criando uma B-Tree...\n");

        /* Inicio do algoritmo de driver */
          criaBT(index); //Cria a raiz e o cabecalho
          index = fopen("arvore.idx", "rb+");
          if(!index) {
            printf("Erro ao abrir o arquivo de indice na Funcionalidade 2! (arvore.idx)\n");
            break;
          }

          // Grava que a B-Tree esta sendo criada esta sendo executada no arquivo de log.
          log_indice(logTxt);

          // Pula o cabecalho do arquivo de dados e vai para o primeiro dado.
          fseek(arq, sizeof(CABECALHO_DADOS), SEEK_SET);

          //Tras o cabecalho da b-tree para a memoria.
          fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

          // Le o tamanho do registro.
          fread(&size, sizeof(size), 1, arq);

          /* Pego o endereco e subtraio o valor da varivel de tamanho, ja que
          esta valor nao entra do calculo do registro */
          chaveInserida.offset = ftell(arq) - sizeof(size);

          // Criar um buffer para armaezenar o registro.
          fread(buffer, size, 1, arq);

          // Divide o registro ate o divisor de campos, e aramezena o id, titulo e genero.
          pos = 0;
          sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);
          r.id = chaveInserida.id; // Usada para guardar a chave para que possamos gravra no log.
          strcpy(r.titulo, parser(buffer, &pos));
          strcpy(r.genero, parser(buffer, &pos));

          // Cria uma chave para ser utilizada como parametro de retorno da recursao.
          chavePromovida.id = -1;
          chavePromovida.offset = -1;

          //Escreve no arquivo de indice que ele esta desatualizado, ficara assim ate inserirmos todos registros
          desatualizarBTree(index, cabecalhoTree);

          // Insere o no raiz;
          log_insercao(logTxt, &chaveInserida, 0, NULL, &r);
          return_log =  inserirBT(index, logTxt, 0, &chaveInserida, &chavePromovida, &invalido, &(cabecalhoTree.contadorDePaginas));
          if(return_log == ERROR) {
                log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
          }

          else {
                log_insercao(logTxt, NULL, 3, NULL, &r);
          }

          while(fread(&size, sizeof(size), 1, arq)) {

            // Pega o proximo registro para inserir na b-tree;
            chaveInserida.offset = (int)ftell(arq) - sizeof(size);
            fread(buffer, size, 1, arq);
            pos = 0;
            sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);
            r.id = chaveInserida.id; // Usada termos um registro completo para que possamos gravar no log para atender as especificacoes.
            strcpy(r.titulo, parser(buffer, &pos));
            strcpy(r.genero, parser(buffer, &pos));

            //Coloca os valores em default.
            chavePromovida.id = -1;
            chavePromovida.offset = -1;
            promo_r_child = -1;

            root = cabecalhoTree.noRaiz;

            log_insercao(logTxt, &chaveInserida, 0, NULL, &r);

            return_log = inserirBT(index, logTxt, root, &chaveInserida, &chavePromovida, &promo_r_child, &(cabecalhoTree.contadorDePaginas));

            if(return_log == ERROR) log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
            else log_insercao(logTxt, NULL, 3, NULL, &r);

            if(return_log == PROMOTION) {

              cabecalhoTree.contadorDePaginas++;
              newRoot.RRNDaPagina = cabecalhoTree.contadorDePaginas - 1;

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
          //Aqui atualizamos a B-Tree apos inserirmos todos os registros
          atualizarBTree(index, cabecalhoTree);
          fseek(index, 0, 0);
          fwrite(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

          fclose(index);
          fclose(arq);

        break;
        }

        //Nesse caso ja temos o arquivo de indice, entao basta adicionar o ultimo registro inserido no arquivo de dados
        else {

            //Traz o cabecalho da b-tree para a memoria.
            fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

            //Se a B-Tree esta atualizada apenas adicionamos uma chave nela
            if(cabecalhoTree.estaAtualizada == TRUE){

                // Vai para o comeco do arquivo de dados.
                fseek(arq, 0, SEEK_SET);

                // Busca informacao do ultimo registro do cabecalho.
                fread(&cabecalhoDados, sizeof(CABECALHO_DADOS), 1, arq);
                fseek(arq, cabecalhoDados.byteoffset_ultimo, SEEK_SET);

                fread(&size, sizeof(size), 1, arq);
                fread(buffer, size, 1, arq);
                pos = 0;
                sscanf(parser(buffer, &pos), "%d", &r.id);
                chaveInserida.id = r.id;
                chaveInserida.offset = cabecalhoDados.byteoffset_ultimo;
                strcpy(r.titulo, parser(buffer, &pos));
                strcpy(r.genero, parser(buffer, &pos));

                //Coloca os valores em default.
                chavePromovida.id = -1;
                chavePromovida.offset = -1;
                promo_r_child = -1;

                //A raiz da arvore eh lida a partir do cabecalho do arquivo de indice e trazida para memoria principal para maior agilidade de mudanca de dados
                root = cabecalhoTree.noRaiz;
                log_insercao(logTxt, &chaveInserida, 0, NULL, &r);

                desatualizarBTree(index, cabecalhoTree);

                return_log = inserirBT(index, logTxt, root, &chaveInserida, &chavePromovida, &promo_r_child, &(cabecalhoTree.contadorDePaginas));

                if(return_log == ERROR) {
                    printf("\nJa existe uma musica com esse ID nos arquivos!\n");
                    log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
                }
                else {
                    printf("\nMusica inserida com sucesso!\n");
                    log_insercao(logTxt, NULL, 3, NULL, &r);
                }

                if(return_log == PROMOTION) {

                    PAGINA newRoot;

                    cabecalhoTree.contadorDePaginas++;
                    newRoot.RRNDaPagina = cabecalhoTree.contadorDePaginas - 1;

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

                atualizarBTree(index, cabecalhoTree);

                fseek(index, 0, 0);
                fwrite(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

                fclose(arq);
                fclose(index);

                break;
            }

            //Se a B-Tree nao estiver atualizada a reconstruimos integralmente a partir do arquivo de dados
            else {

                printf("Arvore desatualizada! Criando uma B-Tree...\n");

                /* Inicio do algoritmo de driver */
                criaBT(index); //Cria a raiz e o cabecalho
                index = fopen("arvore.idx", "rb+");
                if(!index) {
                    printf("Erro ao abrir o arquivo de indice na Funcionalidade 2! (arvore.idx)\n");
                    break;
                }

                // Grava que a B-Tree esta sendo criada esta sendo executada no arquivo de log.
                log_indice(logTxt);

                // Pula o cabecalho do arquivo de dados e vai para o primeiro dado.
                fseek(arq, sizeof(CABECALHO_DADOS), SEEK_SET);

                //Traz o cabecalho da b-tree para a memoria.
                fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

                // Le o tamanho do registro.
                fread(&size, sizeof(size), 1, arq);

                /* Pego o endereco e subtraio o valor da varivel de tamanho, ja que
                esta valor nao entra do calculo do registro */
                chaveInserida.offset = ftell(arq) - sizeof(size);

                // Criar um buffer para armaezenar o registro.
                fread(buffer, size, 1, arq);

                // Divide o registro ate o divisor de campos, e aramezena o id, titulo e genero.
                pos = 0;
                sscanf(parser(buffer, &pos), "%d", &chaveInserida.id);
                r.id = chaveInserida.id; // Usada para guardar a chave para que possamos gravra no log.
                strcpy(r.titulo, parser(buffer, &pos));
                strcpy(r.genero, parser(buffer, &pos));

                // Cria uma chave para ser utilizada como parametro de retorno da recursao.
                chavePromovida.id = -1;
                chavePromovida.offset = -1;

                //Deixaremos a B-Tree desatualizada ate construirmos ela integralmente
                desatualizarBTree(index, cabecalhoTree);

                // Insere o no raiz;
                log_insercao(logTxt, &chaveInserida, 0, NULL, &r);
                return_log =  inserirBT(index, logTxt, 0, &chaveInserida, &chavePromovida, &invalido, &(cabecalhoTree.contadorDePaginas));
                if(return_log == ERROR) {
                        log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
                }

                else {
                        log_insercao(logTxt, NULL, 3, NULL, &r);
                }

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
                    promo_r_child = -1;

                    root = cabecalhoTree.noRaiz;

                    log_insercao(logTxt, &chaveInserida, 0, NULL, &r);

                    return_log = inserirBT(index, logTxt, root, &chaveInserida, &chavePromovida, &promo_r_child, &(cabecalhoTree.contadorDePaginas));

                    if(return_log == ERROR) log_insercao(logTxt, &chaveInserida, 4, NULL, NULL);
                    else log_insercao(logTxt, NULL, 3, NULL, &r);

                    if(return_log == PROMOTION) {
                        cabecalhoTree.contadorDePaginas++;

                        newRoot.RRNDaPagina = cabecalhoTree.contadorDePaginas - 1;

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
                //Atualizamos a B-Tree apenas apos inserir todas as chaves
                atualizarBTree(index, cabecalhoTree);
                fseek(index, 0, 0);
                fwrite(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);

                fclose(index);
                fclose(arq);

                break;
            }
        }

      /* Funcionalidade 3 - Pesquisa (busca)	por	Id	da	m�sica */
      case 3:

        index = fopen("arvore.idx", "rb+");
        if(!index){
            printf("\nNao eh possivel realizar a busca (arquivo de indice nao existe ou ocorreu um erro na sua abertura)\n");
            break;
        }
        //Traz o cabecalho da b-tree para a memoria.
        fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);
        root = cabecalhoTree.noRaiz;

        printf("\nDigite o ID da musica desejada e pressione ENTER: ");
        scanf("%d", &idProcurado);
        clean_stdin();

        //Escreve no arquivo de log que a operacao de busca esta sendo realizada
        log_busca(logTxt, idProcurado);

        //Escreve no arquivo de log se a busca foi um sucesso ou o ID nao foi encontrado
        if(buscaBT(index, root, idProcurado, &offsetProcurado) == TRUE){
            lerRegistro(arq, &r, offsetProcurado);
            printf("\nMusica encontrada!\nID: %d Titulo: %s Genero: %s\n", r.id, r.titulo, r.genero);
            log_sucessoBusca(logTxt, r, offsetProcurado);
        }
        else{
            log_falhaBusca(logTxt, idProcurado);
            printf("\nA musica de ID %d nao foi encontrada\n", idProcurado);
        }


        break;

      /* Funcionalidade 4 - Remocao	de musica	a	partir	do	Id */
      case 4:
          ler_btree(index);
        break;
      /* Funcionalidade 5 - Mostrar	 �rvore-B */
      case 5:
          arq = fopen("dados.dad", "rb+");
          imprimirArquivoDados(arq);
          fclose(arq);
        break;
      /* Sair do programa */
      case 6:
        printf("\nTchau! =)\n");
        end = TRUE;
        break;

      case 7:

        index = fopen("arvore.idx", "rb+");
        if(!index) {
            printf("Erro ao abrir o arquivo de index na Funcionalidade <Mostrar Arvore-B>! (arvore.idx)\nSe a arvore ainda nao existir, insira algumas musicas com a Funcionalidade 2\n");
            break;
        }
        log_funcaoExibirCabecalho(logTxt);

        //Traz o cabecalho da b-tree para a memoria.
        fread(&cabecalhoTree, sizeof(CABECALHO_BTREE), 1, index);
        root = cabecalhoTree.noRaiz;
        exibirBT(index, logTxt, root);

        fclose(index);

        break;

      /* Entrada default */
      default:
        printf("\nComando invalido! Tente novamente.\n");
        break;
    }
  }
  return 0;
}
