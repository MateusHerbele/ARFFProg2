#ifndef __LOG__
#define __LOG__

#include "arff.h"

#define LINE_SIZE_DADOS 2048

char* coletaLinhaDeDados(FILE* arff);
char** separarDados(char* linhaDados, int numAtributos);
unsigned int verificaPosicaoAtributo(atributo* listaDeAtributos, int numAtributos, char* atributo);
unsigned int contaCategorias(char** categorias);
void relatorioDeAtaque(FILE*arff, atributo* vetorAtributos, int numAtributos);
void gerarRelatorioDeAtaque(char** categorias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias);

#endif