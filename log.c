#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

// analisa o arquivo arff e valida para a geração do relatório de ataques
void relatorioDeAtaque(FILE*arff, atributo* vetorAtributos, int numAtributos){
    char** dados = NULL;
    char* linhaDados = NULL;
    unsigned int posicaoAtributo = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_CLASS"); // encontra a posição do atributo PKT_CLASS
    unsigned int numeroDeCategorias = contaCategorias(vetorAtributos[posicaoAtributo].categorias);
    unsigned int copiaNumeroDeCategorias = numeroDeCategorias; // copia para preservar o valor original e ter ele de referência
    unsigned int* numeroDeOcorrencias = calloc(sizeof(unsigned int), numeroDeCategorias); // faz o vetor de número de ocorrências
    if(!numeroDeOcorrencias){
        fprintf(stderr, VRM "Falha ao alocar memória para o vetor de número de ocorrências\n" RESET);
        exit(1);
    }
    while(1){
        linhaDados = coletaLinhaDeDados(arff);
        if(!linhaDados) // se a linha for nula, significa que chegou ao fim do arquivo
            break; 
        dados = separarDados(linhaDados, numAtributos);
        while(copiaNumeroDeCategorias > 0){ // enquanto o número de categorias não chegar a 0
            if(strcmp(dados[posicaoAtributo], vetorAtributos[posicaoAtributo].categorias[copiaNumeroDeCategorias-1]) == 0){ // se o dado da linha for igual a categoria analisada no momento
                if(!(strcmp(dados[posicaoAtributo], "Normal") == 0)){ // se o dado da linha for diferente de normal
                    numeroDeOcorrencias[copiaNumeroDeCategorias-1]++; // incrementa o número de ocorrências da categoria
                    break;
                }else{
                    break; // caso seja normal já pode parar a análise
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
// gera o relatório de ataques
void gerarRelatorioDeAtaque(char** categorias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias){
    FILE* arq = fopen("R_ATAQUES.txt", "w");
    if(!arq){
        perror(VRM "Falha ao abrir o arquivo de relatório de ataques\n" RESET);
        exit(1);
    } 
    for(int i = 0; i < numeroDeCategorias; i++){
        if(strcmp(categorias[i], "Normal") != 0)
            fprintf(arq, "%s;%d\n", categorias[i], numeroDeOcorrencias[i]); // como a posição da categoria é a mesma do número de ocorrências, basta por no arquivo
    }  
    fclose(arq); 
}
// analisa o arquivo arff e valida para a geração do relatório de entidades
void relatorioDeEntidades(FILE*arff, atributo* vetorAtributos, int numAtributos, int ehBlackList){
    char** dados = NULL;
    char* linhaDados = NULL;
    entidades* vetorDeEntidades = NULL;
    unsigned int numeroDeEntidades = 0;
    unsigned int posicaoAtributoAtaques = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_CLASS"); // encontra a posição do atributo PKT_CLASS
    unsigned int numeroDeCategorias = contaCategorias(vetorAtributos[posicaoAtributoAtaques].categorias);
    unsigned int posicaoAtributoEnderecoOrigem = verificaPosicaoAtributo(vetorAtributos, numAtributos, "SRC_ADD"); // encontra a posição do atributo SRC_ADD
    unsigned int copiaNumeroDeCategorias = numeroDeCategorias; // copia para preservar o valor original e ter ele de referência
    unsigned int posicaoEntidade = 0;
    
    while(1){
        linhaDados = coletaLinhaDeDados(arff);
        if(!linhaDados) // se a linha for nula, significa que chegou ao fim do arquivo
            break; 
        dados = separarDados(linhaDados, numAtributos);
        posicaoEntidade = confereEntidades(&vetorDeEntidades, &numeroDeEntidades, dados[posicaoAtributoEnderecoOrigem]); // confere se o endereço parcial analisado no momento já está na lista de entidades
        while(copiaNumeroDeCategorias > 0){ // enquanto o número de categorias não chegar a 0
            if(strcmp(dados[posicaoAtributoAtaques], vetorAtributos[posicaoAtributoAtaques].categorias[copiaNumeroDeCategorias-1]) == 0){ // se o dado da linha for igual a categoria analisada no momento
                if(!(strcmp(dados[posicaoAtributoAtaques], "Normal") == 0)){ // se o dado da linha for diferente de normal
                    vetorDeEntidades[posicaoEntidade].ocorrenciasMaliciosas++; // incrementa o número de ocorrências maliciosas da entidade
                    break;
                }else{
                    break; // caso seja normal já pode parar a análise da linha atual
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
    if(ehBlackList) // caso o identificador de blackList seja verdadeiro, gera a black list ao invés do relatório de entidades
        gerarBlackList(vetorDeEntidades, numeroDeEntidades);
    else
        gerarRelatorioDeEntidades(vetorDeEntidades, numeroDeEntidades);
    
    for(unsigned int i = 0; i < numeroDeEntidades; i++){
        free(vetorDeEntidades[i].ipParcial);
    }
    free(vetorDeEntidades);
}
// confere se o endereço parcial analisado no momento já está na lista de entidades
unsigned int confereEntidades(entidades** vetorDeEntidades, unsigned int* numeroDeEntidades, char* enderecoOrigem){
    if(*numeroDeEntidades == 0){ // se o número de entidades for 0, aloca memória para o vetor de entidades e adiciona a primeira entidade
        (*numeroDeEntidades)++;
        *vetorDeEntidades = malloc(sizeof(entidades) * *numeroDeEntidades);
        if(!vetorDeEntidades){
            fprintf(stderr, "Falha ao alocar memória para a lista de entidades\n");
            exit(1);
        }
        (*vetorDeEntidades)[*numeroDeEntidades-1].ipParcial = strdup(enderecoOrigem);
        (*vetorDeEntidades)[*numeroDeEntidades-1].ocorrenciasMaliciosas = 0;
        return *numeroDeEntidades-1;
    }
    for(unsigned int i = 0; i < *numeroDeEntidades; i++){
        if(strcmp((*vetorDeEntidades)[i].ipParcial, enderecoOrigem) == 0){ // se o endereço parcial analisado no momento já estiver na lista de entidades, retorna a posição dele
            return i;
        }
    }
    (*numeroDeEntidades)++; // se não estiver na lista de entidades, realoca memória para o vetor de entidades e adiciona a entidade
    *vetorDeEntidades = realloc(*vetorDeEntidades, sizeof(entidades) * *numeroDeEntidades);
    if(!vetorDeEntidades){
        fprintf(stderr, "Falha ao realocar memória para a lista de entidades\n");
        exit(1);
    }
    (*vetorDeEntidades)[*numeroDeEntidades-1].ipParcial = strdup(enderecoOrigem);
    (*vetorDeEntidades)[*numeroDeEntidades-1].ocorrenciasMaliciosas = 0;
    return *numeroDeEntidades-1;
}
// gera o relatório de entidades
void gerarRelatorioDeEntidades(entidades* vetorDeEntidades, unsigned int numeroDeEntidades){
    FILE* arq = fopen("R_ENTIDADES.txt", "w");
    if(!arq){
        perror(VRM "Falha ao abrir o arquivo de relatório de entidades\n" RESET);
        exit(1);
    } 
    for(int i = 0; i < numeroDeEntidades; i++){ // acessa a struct de entidades e escreve no arquivo de saída a entidade e sua classificação
        if(vetorDeEntidades[i].ocorrenciasMaliciosas == 0){
            fprintf(arq, "%s;%s\n", vetorDeEntidades[i].ipParcial, "benigna");
        }else if(vetorDeEntidades[i].ocorrenciasMaliciosas <= 5){
            fprintf(arq, "%s;%s\n", vetorDeEntidades[i].ipParcial, "potencialmente maliciosa");
        }else
            fprintf(arq, "%s;%s\n", vetorDeEntidades[i].ipParcial, "maliciosa");
    }
    fclose(arq); 
}
// analisa o arquivo arff e valida para a geração do relatório de médias
void relatorioMedias(FILE*arff, atributo* vetorAtributos, int numAtributos){
    char** dados = NULL;
    char* linhaDados = NULL;
    unsigned int posicaoAtributoAtaques = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_CLASS"); // encontra a posição do atributo PKT_CLASS
    unsigned int numeroDeCategorias = contaCategorias(vetorAtributos[posicaoAtributoAtaques].categorias);
    unsigned int posicaoAtributoMedia = verificaPosicaoAtributo(vetorAtributos, numAtributos, "PKT_AVG_SIZE"); // encontra a posição do atributo PKT_AVG_SIZE
    unsigned int copiaNumeroDeCategorias = numeroDeCategorias; // copia para preservar o valor original e ter ele de referência
    double* mediaMediaAtaques = calloc(sizeof(double), numeroDeCategorias); // faz o vetor de média de média do tamanho de ataques
    if(!mediaMediaAtaques){
        fprintf(stderr, VRM "Falha ao alocar memória para o vetor de média de média de ataques\n" RESET);
        exit(1);
    }
    unsigned int* numeroDeOcorrencias = calloc(sizeof(unsigned int), numeroDeCategorias); // faz o vetor de número de ocorrências de cada ataque
    if(!numeroDeOcorrencias){
        fprintf(stderr, VRM "Falha ao alocar memória para o vetor de número de ocorrências\n" RESET);
        exit(1);
    }
    while(1){
        linhaDados = coletaLinhaDeDados(arff);
        if(!linhaDados) // se a linha for nula, significa que chegou ao fim do arquivo
            break; 
        dados = separarDados(linhaDados, numAtributos);
        while(copiaNumeroDeCategorias > 0){ // enquanto o número de categorias não chegar a 0
            if(strcmp(dados[posicaoAtributoAtaques], vetorAtributos[posicaoAtributoAtaques].categorias[copiaNumeroDeCategorias-1]) == 0){ // se o dado da linha for igual a categoria analisada no momento
                if(!(strcmp(dados[posicaoAtributoAtaques], "Normal") == 0)){ // se o dado da linha for diferente de normal
                    mediaMediaAtaques[copiaNumeroDeCategorias-1] += atof(dados[posicaoAtributoMedia]); // pega o dado ligado a média dos pacotes, e soma com a média de média de ataques da categoria
                    numeroDeOcorrencias[copiaNumeroDeCategorias-1]++; // incrementa o número de ocorrências da categoria
                    break;
                }else{
                    break; // caso seja normal já pode parar a análise da linha atual
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
// gera o relatório de médias
void gerarRelatorioDeMedias(char** categorias, double* somaDasMedias, unsigned int* numeroDeOcorrencias, unsigned int numeroDeCategorias){
    FILE* arq = fopen("R_TAMANHO.txt", "w");
    if(!arq){
        perror("Falha ao abrir o arquivo de relatório de médias\n");
        exit(1);
    }
    for(int i = 0; i < numeroDeCategorias; i++){
        if(strcmp(categorias[i], "Normal") != 0){ // se a categoria for diferente de normal
            if(numeroDeOcorrencias[i] != 0){ // a média real só é feita aqui para poder fazer a comparação com 0 e evitar divisão por 0
                fprintf(arq, "%s;%.2f\n", categorias[i], somaDasMedias[i]/numeroDeOcorrencias[i]);
            }else
                fprintf(arq, "%s;%d\n", categorias[i], 0); // caso o número de ocorrências seja 0, a média é 0
        }
    }
    fclose(arq);
}
// gera a black list com base na função que gera o relatório de entidades
void gerarBlackList(entidades* vetorDeEntidades, unsigned int numeroDeEntidades){
    FILE* arq = fopen("BLACKLIST.bl", "w");
    if(!arq){
        perror("Falha ao abrir o arquivo de relatório de entidades\n");
        exit(1);
    } 
    for(int i = 0; i < numeroDeEntidades; i++)
        if(vetorDeEntidades[i].ocorrenciasMaliciosas > 5) // se o número de ocorrências maliciosas for maior que 5, escreve no arquivo de saída
            fprintf(arq, "%s\n", vetorDeEntidades[i].ipParcial);
    
    fclose(arq); 
}