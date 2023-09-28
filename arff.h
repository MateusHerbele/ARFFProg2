#ifndef __ARFF__
#define __ARFF__

typedef struct {
  char *rotulo;
  char *tipo;
  char **categorias;
} atributo;

#define TAM_LIN_ATR 1024
#define TAM_LIN_DADOS 2048
// Define cores ANSI para uso no printf
#define RESET   "\x1B[0m"
#define PRETO   "\x1B[30m"
#define VRM     "\x1B[31m"
#define VRD   "\x1B[32m"
#define AML  "\x1B[33m"
#define AZL    "\x1B[34m" 
#define MGT "\x1B[35m"
#define CIANO    "\x1B[36m"

// para remover os '\n' do começo/final da string
void strtrim(char *linha);
// coleta a linha de dados com o tamanho máximo de TAM_LIN_DADOS            
char* coletaLinhaDeDados(FILE* arff);
// separa os dados da linha com base no número de atributos
char** separarDados(char* linhaDados, int numAtributos);
// encontra a posição do atributo na lista de atributos
unsigned int verificaPosicaoAtributo(atributo* listaDeAtributos, int numAtributos, char* atributo);
// conta quantas categorias um atributo do tipo categórico tem
unsigned int contaCategorias(char** categorias);
// mostra no stdout os atributos do arquivo
void exibe_atributos(atributo *infos, int quantidade);
// conta o número de atributos do arquivo
int conta_atributos(FILE *arff);
// separa as categorias do atributo no vetor de categorias dele
void processa_categorias(atributo *elemento, char *categorias);
// processa os atributos do arquivo, validando cada um deles e retornando um vetor de atributos
int percorreAtributos(FILE *arff);
// simliar a contaAtributos, porém não retorna o arquivo pro  começo
atributo* processa_atributos(FILE *arff, int quantidade);
// valida todo o arquivo arff, desde os atributos até os dados
void valida_arff(FILE *arff, atributo *atributos, int quantidade);
// libera a memória alocada para os atributos e para o vetor
void liberarMemoria(atributo* atributosArff, int numAtributos);

#endif