#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

// nome_do_ataque;numero_de_ocorrências
void relatorioDeAtaque(FILE*arff, atributo* vetorAtributos, int numAtributos){
    printf("Relatório de ataques\n");
    char** dados = NULL;
    char* linhaDados = NULL;
    unsigned int posicaoAtributo = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_CLASS");
    unsigned int numeroDeCategorias = contaCategorias(vetorAtributos[posicaoAtributo].categorias);
    unsigned int copiaNumeroDeCategorias = numeroDeCategorias;
    unsigned int* numeroDeOcorrencias = calloc(sizeof(unsigned int), numeroDeCategorias);
    if(!numeroDeOcorrencias){
        perror("Falha ao alocar memória para o vetor de número de ocorrências\n");
        exit(1);
    }
    while(1){
        linhaDados = coletaLinhaDeDados(arff);
        if(!linhaDados)
            break; 
        dados = separarDados(linhaDados, numAtributos);
        while(copiaNumeroDeCategorias > 0){
            if(strcmp(dados[posicaoAtributo], vetorAtributos[posicaoAtributo].categorias[copiaNumeroDeCategorias-1]) == 0){
                if(!(strcmp(dados[posicaoAtributo], "Normal") == 0)){
                    numeroDeOcorrencias[copiaNumeroDeCategorias-1]++;
                    break;
                }else{
                    break;
                }
            }
            copiaNumeroDeCategorias--;
        }
        for(int i = 0; i < numAtributos; i++){
        free(dados[i]);
        }
        free(dados);
        free(linhaDados);
        copiaNumeroDeCategorias = numeroDeCategorias;
    }
    gerarRelatorioDeAtaque(vetorAtributos[posicaoAtributo].categorias, numeroDeOcorrencias, numeroDeCategorias);
    free(numeroDeOcorrencias);
}

void gerarRelatorioDeAtaque(char** categorias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias){
    FILE* arq = fopen("R_ATAQUES.txt", "w");
    if(!arq){
        perror("Falha ao abrir o arquivo de relatório de ataques\n");
        exit(1);
    } 
    for(int i = 0; i < numeroDeCategorias; i++){
        if(strcmp(categorias[i], "Normal") != 0)
            fprintf(arq, "%s;%d\n", categorias[i], numeroDeOcorrencias[i]);
    }  
    fclose(arq); 
}

// endereço_origem;classificação
void relatorioDeEntidades(FILE*arff, atributo* vetorAtributos, int numAtributos, int ehBlackList){
    printf("Relatório de entidades\n");
    char** dados = NULL;
    char* linhaDados = NULL;
    entidades* vetorDeEntidades = NULL;
    unsigned int numeroDeEntidades = 0;
    unsigned int posicaoAtributoAtaques = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_CLASS");
    unsigned int numeroDeCategorias = contaCategorias(vetorAtributos[posicaoAtributoAtaques].categorias);
    unsigned int posicaoAtributoEnderecoOrigem = verificaPosicaoAtributo(vetorAtributos, numAtributos, "SRC_ADD");
    unsigned int copiaNumeroDeCategorias = numeroDeCategorias;
    unsigned int posicaoEntidade = 0;
    // unsigned int numeroDeOcorrencias
    
    while(1){
        linhaDados = coletaLinhaDeDados(arff);
        if(!linhaDados)
            break; 
        dados = separarDados(linhaDados, numAtributos);
        posicaoEntidade = confereEntidades(&vetorDeEntidades, &numeroDeEntidades, dados[posicaoAtributoEnderecoOrigem]);
        while(copiaNumeroDeCategorias > 0){
            if(strcmp(dados[posicaoAtributoAtaques], vetorAtributos[posicaoAtributoAtaques].categorias[copiaNumeroDeCategorias-1]) == 0){
                if(!(strcmp(dados[posicaoAtributoAtaques], "Normal") == 0)){
                    vetorDeEntidades[posicaoEntidade].ocorrenciasMaliciosas++;
                    break;
                }else{
                    break;
                }
            }
            copiaNumeroDeCategorias--;
        }
        for(int i = 0; i < numAtributos; i++){
        free(dados[i]);
        }
        free(dados);
        free(linhaDados);
        copiaNumeroDeCategorias = numeroDeCategorias;
    }
    if(ehBlackList)
        gerarBlackList(vetorDeEntidades, numeroDeEntidades);
    else
        gerarRelatorioDeEntidades(vetorDeEntidades, numeroDeEntidades);
    
    for(unsigned int i = 0; i < numeroDeEntidades; i++){
        free(vetorDeEntidades[i].ipParcial);
    }
    free(vetorDeEntidades);
}

unsigned int confereEntidades(entidades** vetorDeEntidades, unsigned int* numeroDeEntidades, char* enderecoOrigem){
    if(*numeroDeEntidades == 0){
        (*numeroDeEntidades)++;
        *vetorDeEntidades = malloc(sizeof(entidades) * *numeroDeEntidades);
        if(!vetorDeEntidades){
            perror("Falha ao alocar memória para a lista de entidades\n");
            exit(1);
        }
        (*vetorDeEntidades)[*numeroDeEntidades-1].ipParcial = strdup(enderecoOrigem);
        (*vetorDeEntidades)[*numeroDeEntidades-1].ocorrenciasMaliciosas = 0;
        return *numeroDeEntidades-1;
    }
    for(unsigned int i = 0; i < *numeroDeEntidades; i++){
        if(strcmp((*vetorDeEntidades)[i].ipParcial, enderecoOrigem) == 0){
            return i;
        }
    }
    (*numeroDeEntidades)++;
    *vetorDeEntidades = realloc(*vetorDeEntidades, sizeof(entidades) * *numeroDeEntidades);
    if(!vetorDeEntidades){
        perror("Falha ao realocar memória para a lista de entidades\n");
        exit(1);
    }
    (*vetorDeEntidades)[*numeroDeEntidades-1].ipParcial = strdup(enderecoOrigem);
    (*vetorDeEntidades)[*numeroDeEntidades-1].ocorrenciasMaliciosas = 0;
    return *numeroDeEntidades-1;
}

void gerarRelatorioDeEntidades(entidades* vetorDeEntidades, unsigned int numeroDeEntidades){
    FILE* arq = fopen("R_ENTIDADES.txt", "w");
    if(!arq){
        perror("Falha ao abrir o arquivo de relatório de entidades\n");
        exit(1);
    } 
    for(int i = 0; i < numeroDeEntidades; i++){
        if(vetorDeEntidades[i].ocorrenciasMaliciosas == 0){
            fprintf(arq, "%s;%s\n", vetorDeEntidades[i].ipParcial, "benigna");
        }else if(vetorDeEntidades[i].ocorrenciasMaliciosas <= 5){
            fprintf(arq, "%s;%s\n", vetorDeEntidades[i].ipParcial, "potencialmente maliciosa");
        }else
            fprintf(arq, "%s;%s\n", vetorDeEntidades[i].ipParcial, "maliciosa");
    }
    fclose(arq); 
}

// nome_do_ataque; media_media_do_tamanho
void relatorioMedias(FILE*arff, atributo* vetorAtributos, int numAtributos){
    printf("Relatório de médias\n");
    char** dados = NULL;
    char* linhaDados = NULL;
    unsigned int posicaoAtributoAtaques = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_CLASS");
    unsigned int numeroDeCategorias = contaCategorias(vetorAtributos[posicaoAtributoAtaques].categorias);
    unsigned int posicaoAtributoMedia = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_AVG_SIZE");
    unsigned int copiaNumeroDeCategorias = numeroDeCategorias;
    int* mediaMediaAtaques = calloc(sizeof(unsigned int), numeroDeCategorias);
    if(!mediaMediaAtaques){
        perror("Falha ao alocar memória para o vetor de média de média de ataques\n");
        exit(1);
    }
    unsigned int* numeroDeOcorrencias = calloc(sizeof(unsigned int), numeroDeCategorias);
    if(!numeroDeOcorrencias){
        perror("Falha ao alocar memória para o vetor de número de ocorrências\n");
        exit(1);
    }
    while(1){
        linhaDados = coletaLinhaDeDados(arff);
        if(!linhaDados)
            break; 
        dados = separarDados(linhaDados, numAtributos);
        while(copiaNumeroDeCategorias > 0){
            if(strcmp(dados[posicaoAtributoAtaques], vetorAtributos[posicaoAtributoAtaques].categorias[copiaNumeroDeCategorias-1]) == 0){
                if(!(strcmp(dados[posicaoAtributoAtaques], "Normal") == 0)){
                    mediaMediaAtaques[copiaNumeroDeCategorias-1] += atoi(dados[posicaoAtributoMedia]);
                    numeroDeOcorrencias[copiaNumeroDeCategorias-1]++;
                    break;
                }else{
                    break;
                }
            }
            copiaNumeroDeCategorias--;
        }
        for(int i = 0; i < numAtributos; i++){
        free(dados[i]);
        }
        free(dados);
        free(linhaDados);
        copiaNumeroDeCategorias = numeroDeCategorias;
    }
    gerarRelatorioDeMedias(vetorAtributos[posicaoAtributoAtaques].categorias, mediaMediaAtaques, numeroDeOcorrencias, numeroDeCategorias);
    free(mediaMediaAtaques);
    free(numeroDeOcorrencias);
}

void gerarRelatorioDeMedias(char** categorias, int* somaDasMedias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias){
    FILE* arq = fopen("R_TAMANHO.txt", "w");
    if(!arq){
        perror("Falha ao abrir o arquivo de relatório de médias\n");
        exit(1);
    }
    for(int i = 0; i < numeroDeCategorias; i++){
        if(strcmp(categorias[i], "Normal") != 0){
            if(numeroDeOcorrencias[i] != 0){
                fprintf(arq, "%s;%d\n", categorias[i], somaDasMedias[i]/numeroDeOcorrencias[i]);
            }else
                fprintf(arq, "%s;%d\n", categorias[i], 0);
        }
    }
    fclose(arq);
}
// endereço_origem
void gerarBlackList(entidades* vetorDeEntidades, unsigned int numeroDeEntidades){
    printf("Gerando BlackList\n");
    FILE* arq = fopen("BLACKLIST.bl", "w");
    if(!arq){
        perror("Falha ao abrir o arquivo de relatório de entidades\n");
        exit(1);
    } 
    for(int i = 0; i < numeroDeEntidades; i++)
        if(vetorDeEntidades[i].ocorrenciasMaliciosas > 5)
            fprintf(arq, "%s\n", vetorDeEntidades[i].ipParcial);
    
    fclose(arq); 
}