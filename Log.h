#ifndef LOG_H_
#define LOG_H_

void log_insercao(FILE *arq, CHAVE *chave, int codigo, PAGINA *p, REGISTRO *reg);
void log_indice(FILE *arq);
void log_busca(FILE *arq, CHAVE chave);

#endif // LOG_H_
