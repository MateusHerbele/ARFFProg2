#ifndef __LOG__
#define __LOG__

#include "arff.h"

typedef struct{ 
  char* ipParcial;
  unsigned int ocorrenciasMaliciosas;
}entidades;

// analisa o arquivo arff e valida para a geração do relatório de ataques
void relatorioDeAtaque(FILE*arff, atributo* vetorAtributos, int numAtributos);
// gera o relatório de ataques
void gerarRelatorioDeAtaque(char** categorias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias);
// analisa o arquivo arff e valida para a geração do relatório de entidades
void relatorioDeEntidades(FILE*arff, atributo* vetorAtributos, int numAtributos, int ehBlackList);
// confere se o endereço parcial analisado no momento já está na lista de entidades
unsigned int confereEntidades(entidades** vetorDeEntidades, unsigned int* numeroDeEntidades, char* enderecoOrigem);
// gera o relatório de entidades
void gerarRelatorioDeEntidades(entidades* vetorDeEntidades, unsigned int numeroDeEntidades);
// analisa o arquivo arff e valida para a geração do relatório de médias
void relatorioMedias(FILE*arff, atributo* vetorAtributos, int numAtributos);
// gera o relatório de médias
void gerarRelatorioDeMedias(char** categorias, double* somaDasMedias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias);
// gera a black list com base na função que gera o relatório de entidades
void gerarBlackList(entidades* vetorDeEntidades, unsigned int numeroDeEntidades);

#endif