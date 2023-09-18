#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arff.h"

#define LINE_SIZE_ATRIBUTO 1024
#define LINE_SIZE_DADOS 2048
// Define um contador estático para o macro
static unsigned int contadorDebug = 0;

// Macro para fins de depuração
#define DEBUG_MESSAGE(mensagem) \
        printf("Debug %d: %s\n", ++contadorDebug, mensagem); \


void exibe_atributos(atributo *infos, int quantidade){
  //Fun��o do A1 (com modifica��es para o atributo de categorias)
  unsigned int j = 0; // para percorrer as categorias
  if (infos == 0){  
    printf("O arquivo ARFF fornecido é inválido!\n");
    exit(0);
  }

  printf("==== ATRIBUTOS DO ARQUIVO ====\n");
  for(int i = 0; i < quantidade; i++){
    printf("-> Atributo #%d\n", i+1);
    printf("Rótulo: %s\n", infos[i].rotulo);
    printf("Tipo: %s\n", infos[i].tipo);
    if (infos[i].categorias){
      printf("Categorias: {");
      while(infos[i].categorias[j] != NULL){
        printf("%s", infos[i].categorias[j]);
        if(infos[i].categorias[j+1] != NULL) printf(", ");
        j++;
      }
      j = 0;
    }
    if (i < quantidade-1) printf("------------------------------\n");
  }
  printf("===============================\n");
}

int conta_atributos(FILE *arff){
  //Fun��o do A1
  char percorre[LINE_SIZE_ATRIBUTO+1];
  char* atributoArq = "@attribute"; // para comparacao
  char* token;
  char separador[] = " "; // para separar os atributos/tipos
  int numAtributos = 0;

  while(!feof(arff)){
      fgets(percorre, LINE_SIZE_ATRIBUTO, arff);
      token = strtok(percorre, separador);
      if(strcmp(atributoArq, token) == 0){
          numAtributos += 1;        
      }
      if(strcmp("@data\0", token) == 0 || strcmp("@data\n", token) == 0){
          rewind(arff);
          return numAtributos;
      }
  }
  return 0;
}

void processa_categorias(atributo *elemento, char *categorias){
  //Recbe uma string com as categorias e atualiza o elemento com um vetor de strings (modificar a struct)
  //Eu : vou precisar percorrer a string inteira pra descobrir o número de categorias
  char* token;
  char* percorre = categorias;
  int numCategorias = 1;

  token = strtok(percorre, ",");
  token = token + 1; //para ignorar o '{'
  elemento->categorias = malloc(sizeof(char*) * numCategorias);
  if(!elemento->categorias){
    perror("Falha ao alocar memória para as categorias\n");
    exit(1);
  }
  elemento->categorias[0] = strdup(token);

  while(*percorre != '}'){
    if(*percorre == ',' || *(percorre+1) == '}'){
      token = strtok(NULL, ",");
      numCategorias++;
      elemento->categorias = realloc(elemento->categorias, sizeof(char*) * (numCategorias+1));
      if(!elemento->categorias){
        perror("Falha ao re-alocar memória para as categorias\n");
        exit(1);
      }
      elemento->categorias[numCategorias-1] = strdup(token);
    }
    percorre++;
  }
  elemento->categorias[numCategorias] = NULL; // para indicar o final
}

atributo* processa_atributos(FILE *arff, int quantidade){
  //Fun��o do A1 (com modifica��es para o atributo de categorias)
    atributo* atributosArff;
    char percorre[LINE_SIZE_ATRIBUTO+1];
    char percorreCopia[LINE_SIZE_ATRIBUTO+1];
    char separador[] = " "; // para separar os atributos/tipos
    char* token;
    int numAtributos = conta_atributos(arff);
    int i = 0;
    
    atributosArff = malloc(sizeof(atributo) * numAtributos);
    if(!atributosArff){
        perror("falha ao alocar memória\n");
        exit(1);
    }

    while(!feof(arff)){
        fgets(percorre, LINE_SIZE_ATRIBUTO, arff);
        strcpy(percorreCopia, percorre);
        token = strtok(percorre, separador);
        if(strcmp("@attribute", token) == 0){
            //rotulo
            token = strtok(NULL, separador);
            atributosArff[i].rotulo = strdup(token);
            if(!atributosArff[i].rotulo){
                perror("falha ao alocar memória ao duplicar o token para o rótulo\n");
                exit(1);
            }
            //tipo
            token = strtok(NULL, separador);
            if((strcmp("numeric\n", token) == 0 )||(strcmp(token, "string\n") == 0)){
                atributosArff[i].tipo = strdup(token);
                if(!atributosArff[i].tipo){
                    perror("falha ao alocar memória ao duplicar o token para o tipo\n");
                    exit(1);
                }
                atributosArff[i].categorias = NULL;
            }else{
                if(token[0] == '{'){ // assumindo que as categorias estao corretamente escritas
                  //categorias
                  atributosArff[i].tipo = "categoric";
                  processa_categorias(&atributosArff[i], strdup(token)); // soma para ignorar o @attribute e o rótulo    
                }else{
                  perror("Atributo inválido\n");
                  exit(1);
                }
            }
            i++;
        }else{
          if(strcmp("@data\0", token) == 0 || strcmp("@data\n", token) == 0)
            return atributosArff;
          else 
          if(percorre[0] != '\n'){
            perror("Atributo inválido\n");
            exit(1);
          }
        }
        
    }
    return 0;
}
  
void valida_arff(FILE *arff, atributo *atributos, int quantidade){
  //Recebe um arquivo arff com ponteiro de leitura antes do "@data"; passa por todas as linhas de dados e valida cada elementos de cada coluna em
  //rela��o ao vetor de atributos tamb�m fornecido como argumento.
}

// libera a memória alocada para os atributos e para o vetor
void liberarMemoria(atributo* atributosArff, int numAtributos){
    unsigned int j = 0;
    for(int i = 0; i < numAtributos; i++){
        free(atributosArff[i].tipo);
        free(atributosArff[i].rotulo);
        if(atributosArff[i].categorias){
          while(atributosArff[i].categorias[j]){
            free(atributosArff[i].categorias[j]); // libera o j e incrementa pro próximo loop
            j++;
          }
        free(atributosArff[i].categorias);
        j = 0;
        }
    }
    free(atributosArff);
}
