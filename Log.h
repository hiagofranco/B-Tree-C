#ifndef LOG_H_
#define LOG_H_

void log_insercao(FILE *arq, CHAVE *chave, int codigo, PAGINA *p, REGISTRO *reg);
void log_indice(FILE *arq);
void log_busca(FILE *arq, int id);
void log_sucessoBusca(FILE *arq, REGISTRO r, long int offset);
void log_falhaBusca(FILE *arq, int id);
void log_funcaoExibirCabecalho(FILE *arq);
void log_exibirBTree(FILE *log, PAGINA p, int nivel);

#endif // LOG_H_
