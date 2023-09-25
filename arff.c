#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arff.h"

// para remover os '\n' do começo/final da string
void strtrim(char *linha){
    unsigned int tamanho = strlen(linha);
    unsigned int comeco = 0;
    unsigned int final = tamanho - 1;
    // se a linha for vazia
    if(linha[0] == '\n'){
        return;
    }
    // começo
    while(comeco < tamanho && linha[comeco] == '\n'){
        comeco++;
    }
    // final
    while(tamanho >= 0 && linha[final] == '\n'){
        final--;
    }
    // altera a string
    if(comeco > 0 || final < tamanho -1){
        memmove(linha, linha + comeco, final - comeco + 1);
        linha[final - comeco + 1] = '\0';
    }
}
            
char* coletaLinhaDeDados(FILE* arff){
    char percorre[LINE_SIZE_DADOS+1];
  
   if(fgets(percorre, LINE_SIZE_DADOS, arff) == NULL){
        return NULL;
    }
    
    if(percorre[0] == '\n'){
        return coletaLinhaDeDados(arff);
    }

    return strdup(percorre);
}

char** separarDados(char* linhaDados, int numAtributos){
    char* token = NULL;
    char separador[] = ",";

    int i = 0;
    char** dados = (char**)malloc(sizeof(char*)*numAtributos);
    if(!dados){
        perror("Falha ao alocar memória para os dados\n");
        exit(1);
    }

    token = strtok(linhaDados, separador);
    while(token != NULL){
        strtrim(token);
        dados[i] = strdup(token);
        if(!dados[i]){
            perror("Falha ao alocar memória para os dados ao duplicar o token\n");
            exit(1);
        }
        token = strtok(NULL, separador);
        i++;
    }
    if(i != numAtributos){
        printf("Erro na linha de dados: %s\n", linhaDados);
        perror("Número de dados não confere com o número de atributos.\n");
        exit(1);
    }
    return dados;
}

unsigned int verificaPosicaoAtributo(atributo* listaDeAtributos, int numAtributos, char* atributo){
    for(unsigned int i = 0; i < numAtributos; i++){
        if(strcmp(listaDeAtributos[i].rotulo, atributo) == 0){
            return i;
        }
    }
    printf("Atributo %s não encontrado, fechando a execução\n", atributo);
    perror("Atributo não encontrado ao verificar a posição deste.\n");
    exit(1);
}

unsigned int contaCategorias(char** categorias){
    unsigned int i = 0;
    char** copiaCategorias = categorias;
    while(copiaCategorias[i] != NULL){
        i++;
    }
    return i;
}

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
      printf("}\n");
    }
    if (i < quantidade-1) printf("------------------------------\n");
  }
  printf("===============================\n");
}

int conta_atributos(FILE *arff){
  //Fun��o do A1
  char percorre[LINE_SIZE_ATRIBUTO+1];
  char* token = NULL; // mudança dia 21/09 esse null
  char separador[] = " "; // para separar os atributos/tipos
  int numAtributos = 0;

  while(!feof(arff)){
      fgets(percorre, LINE_SIZE_ATRIBUTO, arff);
      token = strtok(percorre, separador);
      strtrim(token);
      if(strcmp("@attribute", token) == 0){
          numAtributos += 1;        
      }
      if(strcmp("@data", token) == 0){
          rewind(arff);
          return numAtributos;
      }
  }
  return 0;
}

void processa_categorias(atributo *elemento, char *categorias){
  //Recbe uma string com as categorias e atualiza o elemento com um vetor de strings (modificar a struct)
  //Eu : vou precisar percorrer a string inteira pra descobrir o número de categorias
  char* token = NULL;
  char* percorre = categorias;
  unsigned int tamanhoFinal = 0; 
  int numCategorias = 1;

  token = strtok(percorre, ",");
  token = token + 1; //para ignorar o '{'
  elemento->categorias = malloc(sizeof(char*) * numCategorias);
  if(!elemento->categorias){
    perror("Falha ao alocar memória para as categorias\n");
    exit(1);
  }
  elemento->categorias[0] = strdup(token);
  if(!elemento->categorias[0]){
    perror("Falha ao alocar memória ao duplicar o token para as categorias\n");
    exit(1);
  }

  while(*percorre != '}'){
    if(*percorre == ',' || *(percorre+1) == '}'){
      token = strtok(NULL, ",");
      numCategorias++;
      elemento->categorias = realloc(elemento->categorias, sizeof(char*) * (numCategorias+1));
      // elemento->categorias = malloc(sizeof(char*) * (numCategorias+1));
      if(!elemento->categorias){
        perror("Falha ao re-alocar memória para as categorias\n");
        exit(1);
      }
      elemento->categorias[numCategorias-1] = strdup(token);
      if(!elemento->categorias[numCategorias-1]){
        perror("Falha ao alocar memória ao duplicar o token para as categorias\n");
        exit(1);
      }
    }
    percorre++;
  }
  tamanhoFinal = strlen(elemento->categorias[numCategorias-1]);
  elemento->categorias[numCategorias-1][tamanhoFinal-2] = '\0'; // para remover o '\n' do final
  elemento->categorias[numCategorias] = NULL; // para indicar o final
}

atributo* processa_atributos(FILE *arff, int quantidade){
  //Fun��o do A1 (com modifica��es para o atributo de categorias)
    atributo* atributosArff;
    char percorre[LINE_SIZE_ATRIBUTO+1];
    char percorreCopia[LINE_SIZE_ATRIBUTO+1];
    char separador[] = " "; // para separar os atributos/tipos
    char* token;
    char* tokenCopia;
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
                perror("Falha ao alocar memória ao duplicar o token para o rótulo.\n");
                exit(1);
            }
            //tipo
            token = strtok(NULL, separador);
            if(!token){
                printf("Linha apresentando problema: %s\n", percorreCopia);
                perror("Atributo inválido, faltando elementos da linha.\n");
                exit(1);
            }
            if((strcmp("numeric\n", token) == 0 )||(strcmp(token, "string\n") == 0)){
                atributosArff[i].tipo = strdup(token);
                if(!atributosArff[i].tipo){
                    perror("Falha ao alocar memória ao duplicar o token para o tipo.\n");
                    exit(1);
                }
                atributosArff[i].categorias = NULL;
            }else{
                if(token[0] == '{'){ 
                  //categorias
                  atributosArff[i].tipo = strdup("categoric");
                  if(!atributosArff[i].tipo){
                    perror("Falha ao alocar memória ao duplicar o token para o tipo categoric.\n");
                    exit(1);
                  }
                  tokenCopia = strdup(token);
                  if(!tokenCopia){
                    perror("Falha ao alocar memória ao duplicar o token para o processaCategorias.\n");
                    exit(1);
                  }
                  processa_categorias(&atributosArff[i], tokenCopia); 
                  free(tokenCopia);
                }else{
                  printf("Atributo %s não é válido, fechando a execução.\n", token);
                  perror("Atributo inválido\n");
                  exit(1);
                }
            }
            i++;
        }else{
          strtrim(token);
          if(strcmp("@data", token) == 0)
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
  if(quantidade == 0){
    perror("Arquivo inválido devido a não ter atributos.\n");
    exit(1);
  }
  char* linhaDados;
  char** dados = NULL;
  int numeroDeCategorias = 0;
  int j = 0;
  conta_atributos(arff);
  // posData(arff);
  while(1){
    linhaDados = coletaLinhaDeDados(arff);
    if(!linhaDados)
        break; 
    dados = separarDados(linhaDados, quantidade);
    for(unsigned int i = 0; i < quantidade; i++){
      if(strcmp(atributos[i].tipo, "numeric\n") == 0 && atol(dados[i]) == 0 && strcmp(dados[i], "0") != 0){
        if(atof(dados[i]) == 0){ // se numeric for só inteiro, tirar isso aqui
          printf("Arquivo inválido devido ao dado do atributo %s não ser numérico.\n", atributos[i].rotulo);
          perror("Arquivo inválido devido a um valor não numérico.\n");
          exit(1);
        }
      }else if(strcmp(atributos[i].tipo, "string\n") == 0 && dados[i] == NULL){
        printf("Arquivo inválido devido ao dado do atributo %s não ser atribuído.\n", atributos[i].rotulo);
        perror("Arquivo inválido devido a um valor nulo onde era esperado uam string.\n");
        exit(1);
    }else if(strcmp(atributos[i].tipo, "categoric") == 0){
      numeroDeCategorias = contaCategorias(atributos[i].categorias);
      while(numeroDeCategorias > 0){
        if(strcmp(dados[i], atributos[i].categorias[j]) == 0){
          break;
        }
        numeroDeCategorias--;
        j++;
      }
      if(numeroDeCategorias == 0){
        printf("Arquivo inválido devido ao dado do atributo %s não ser uma categoria válida.\n", atributos[i].rotulo);
        perror("Arquivo inválido devido a um valor não ser uma categoria válida.\n");
        exit(1);
      }
      j = 0;
    }
  } 
    for(int i = 0; i < quantidade; i++){
      free(dados[i]);
    }
    free(dados);
    free(linhaDados);
  }
}
// libera a memória alocada para os atributos e para o vetor
void liberarMemoria(atributo* atributosArff, int numAtributos){
    unsigned int j = 0;
    for(int i = 0; i < numAtributos; i++){
        free(atributosArff[i].tipo);
        free(atributosArff[i].rotulo);
        if(atributosArff[i].categorias != NULL){
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
