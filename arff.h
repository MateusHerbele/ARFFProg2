#ifndef __ARFF__
#define __ARFF__

typedef struct {
  char *rotulo;
  char *tipo;
  char **categorias;
} atributo;

#define LINE_SIZE_ATRIBUTO 1024
#define LINE_SIZE_DADOS 2048

void strtrim(char *linha);
char* coletaLinhaDeDados(FILE* arff);
char** separarDados(char* linhaDados, int numAtributos);
unsigned int verificaPosicaoAtributo(atributo* listaDeAtributos, int numAtributos, char* atributo);
unsigned int contaCategorias(char** categorias);
void exibe_atributos(atributo *infos, int quantidade);
int conta_atributos(FILE *arff);
void processa_categorias(atributo *elemento, char *categorias);
atributo* processa_atributos(FILE *arff, int quantidade);
void posData(FILE* arff);
void valida_arff(FILE *arff, atributo *atributos, int quantidade);
void liberarMemoria(atributo* atributosArff, int numAtributos);

#endif