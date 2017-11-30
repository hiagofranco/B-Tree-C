#ifndef LOG_H_
#define LOG_H_

void log_insercao(FILE *arq, CHAVE chave, FILE *dados, int codigo, pagina *p);
void log_indice(FILE *arq);
void log_busca(FILE *arq, CHAVE chave);

#endif // LOG_H_
