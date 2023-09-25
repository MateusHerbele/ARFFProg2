#ifndef __LOG__
#define __LOG__

#include "arff.h"

typedef struct {
  char* ipParcial;
  unsigned int ocorrenciasMaliciosas;
} entidades;

void relatorioDeAtaque(FILE*arff, atributo* vetorAtributos, int numAtributos);
void gerarRelatorioDeAtaque(char** categorias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias);
void relatorioDeEntidades(FILE*arff, atributo* vetorAtributos, int numAtributos, int ehBlackList);
unsigned int confereEntidades(entidades** vetorDeEntidades, unsigned int* numeroDeEntidades, char* enderecoOrigem);
void gerarRelatorioDeEntidades(entidades* vetorDeEntidades, unsigned int numeroDeEntidades);
void relatorioMedias(FILE*arff, atributo* vetorAtributos, int numAtributos);
void gerarRelatorioDeMedias(char** categorias, int* somaDasMedias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias);
void gerarBlackList(entidades* vetorDeEntidades, unsigned int numeroDeEntidades);

#endif