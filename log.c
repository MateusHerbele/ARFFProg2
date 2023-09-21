#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

char* coletaLinhaDeDados(FILE* arff){
    char percorre[LINE_SIZE_DADOS+1];
  
    fgets(percorre, LINE_SIZE_DADOS, arff);
    printf("percorre:%s\n", percorre);
    while(percorre[0] == '\n'){
        printf("Looping 2 coletaLinhaDeDados\n");
        fgets(percorre, LINE_SIZE_DADOS, arff);
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
        printf("Looping 1 separarDados\n");
        dados[i] = strdup(token);
        token = strtok(NULL, separador);
        i++;
    }
    return dados;
}

unsigned int verificaPosicaoAtributo(atributo* listaDeAtributos, int numAtributos, char* atributo){
    for(unsigned int i = 0; i < numAtributos; i++){
        printf("Looping 1 verificaPosicaoAtributo\n");
        if(strcmp(listaDeAtributos[i].rotulo, atributo) == 0){
            return i;
        }
    }
    printf("Atributo %s não encontrado, fechando a execução\n", atributo);
    perror("Atributo não encontrado na ao verificar a posição deste.\n");
    exit(1);
}

unsigned int contaCategorias(char** categorias){
    unsigned int i = 0;
    char** copiaCategorias = categorias;
    while(copiaCategorias[i] != NULL){
        printf("Looping 1 contaCategorias\n");
        i++;
    }
    return i;
}

void relatorioDeAtaque(FILE*arff, atributo* vetorAtributos, int numAtributos){
    char** dados = NULL;
    char* linhaDados = malloc(sizeof(char)*LINE_SIZE_DADOS+1);
    if(!linhaDados){
        perror("Falha ao alocar memória para a linha de dados\n");
        exit(1);
    }
    unsigned int posicaoAtributo = 0;
    unsigned int numeroDeCategorias = 0;
    unsigned int copiaNumeroDeCategorias = 0;
    unsigned int* numeroDeOcorrencias = NULL;

    posicaoAtributo = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_CLASS");
    printf("posicaoAtributo:%d\n", posicaoAtributo);
    printf("no vetor: %s\n", vetorAtributos[posicaoAtributo].rotulo);
    numeroDeCategorias = contaCategorias(vetorAtributos[posicaoAtributo].categorias);
    printf("numeroDeCategorias:%d\n", numeroDeCategorias);
    copiaNumeroDeCategorias = numeroDeCategorias;
    numeroDeOcorrencias = malloc(sizeof(unsigned int)*numeroDeCategorias);
    if(!numeroDeOcorrencias){
        perror("Falha ao alocar memória para o vetor de número de ocorrências\n");
        exit(1);
    }
    while(!feof(arff)){
        linhaDados = coletaLinhaDeDados(arff);
        printf("linha de dados:%s\n", linhaDados);
        dados = separarDados(linhaDados, numAtributos);
        printf("Looping 1 relatorioDeAtaque\n");
        while(copiaNumeroDeCategorias > 0){
            printf("Looping 2 relatorioDeAtaque\n");
            printf("dados:%s\n", dados[posicaoAtributo]);
            if(strcmp(dados[posicaoAtributo], vetorAtributos[posicaoAtributo].categorias[numeroDeCategorias-1]) == 0){
                if(!strcmp(dados[posicaoAtributo], "Normal") == 0){
                    numeroDeOcorrencias[numeroDeCategorias-1]++;
                    break;
                }else{
                    break;
                }
            }
            posicaoAtributo++;
            numeroDeCategorias--;
        }
        copiaNumeroDeCategorias = numeroDeCategorias;
    }
    gerarRelatorioDeAtaque(vetorAtributos[posicaoAtributo].categorias, numeroDeOcorrencias, numeroDeCategorias);
}

void gerarRelatorioDeAtaque(char** categorias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias){
    FILE* arq = fopen("R_ATAQUES.txt", "w");
    if(!arq){
        perror("Falha ao abrir o arquivo de relatório de ataques\n");
        exit(1);
    } 
    for(int i = 0; i < numeroDeCategorias; i++){
        printf("Looping 1 gerarRelatorioDeAtaque\n");
        fprintf(arq, "%s;%d\n", categorias[i], numeroDeOcorrencias[i]);
    }  
    fclose(arq); 
}
// nome_do_ataque;numero_de_ocorrências

// endereço_origem;classificação

// nome_do_ataque; media_media_do_tamanho

// endereço_origem