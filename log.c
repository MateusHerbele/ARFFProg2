#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

// char** pesquisaCategorias(atributo* arff, char charGuia, int numAtributos){
//     switch(charGuia){
//         case 'a':
//             for(int i = 0; i < numAtributos; i++){
//                 if(strcmp(arff[i].rotulo, "PKT_CLASS") == 0){
//                     return arff[i].categorias;
//                 }
//             }
//         break;
//         case 'o':
//             for(int i = 0; i < numAtributos; i++){
//                 if(strcmp(arff[i].rotulo, "ORIGEM") == 0){
//                     return arff[i].categorias;
//                 }
//             }
//         break;
//         case 'p':

//         break;
//     }

// }
char* coletaLinhaDeDados(FILE* arff){
    char percorre[LINE_SIZE_DADOS+1];
    static unsigned int emDados = 0; 
    
    if(emDados == 0)
        while(!feof(arff)){
            fgets(percorre, LINE_SIZE_ATRIBUTO, arff);
            if(strcmp(percorre, "data\n") == 0){
                emDados = 1;
                break;
            }
        }
    while(percorre[0] == '\n'){
        fgets(percorre, LINE_SIZE_ATRIBUTO, arff);
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
        dados[i] = strdup(token);
        token = strtok(NULL, separador);
        i++;
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
    perror("Atributo não encontrado na ao verificar a posição deste.\n");
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
    numeroDeCategorias = contaCategorias(vetorAtributos[posicaoAtributo].categorias);
    copiaNumeroDeCategorias = numeroDeCategorias;
    numeroDeOcorrencias = malloc(sizeof(unsigned int)*numeroDeCategorias);
    if(!numeroDeOcorrencias){
        perror("Falha ao alocar memória para o vetor de número de ocorrências\n");
        exit(1);
    }
    while(!feof(arff)){
        linhaDados = coletaLinhaDeDados(arff);
        dados = separarDados(linhaDados, numAtributos);
        while(copiaNumeroDeCategorias > 0){
            if(strcmp(dados[posicaoAtributo], vetorAtributos[posicaoAtributo].categorias[numeroDeCategorias-1]) == 0){
                numeroDeOcorrencias[numeroDeCategorias-1]++;
                break;
            }
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
        fprintf(arq, "%s;%d\n", categorias[i], numeroDeOcorrencias[i]);
    }  
    fclose(arq); 
}
// nome_do_ataque;numero_de_ocorrências

// endereço_origem;classificação

// nome_do_ataque; media_media_do_tamanho

// endereço_origem