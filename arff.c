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
// coleta a linha de dados com o tamanho máximo de TAM_LIN_DADOS            
char* coletaLinhaDeDados(FILE* arff){
	char percorre[TAM_LIN_DADOS+1];

	if(fgets(percorre, TAM_LIN_DADOS, arff) == NULL){
		return NULL;
	}
	
	if(percorre[0] == '\n'){
		return coletaLinhaDeDados(arff);
	}

	return strdup(percorre);
}
// separa os dados da linha com base no número de atributos
char** separarDados(char* linhaDados, int numAtributos){
	char* token = NULL;
	char separador[] = ",";

	int i = 0;
	char** dados = (char**)malloc(sizeof(char*)*numAtributos);
	if(!dados){
		fprintf(stderr, "Falha ao alocar memória para os dados\n");
		exit(1);
	}

	token = strtok(linhaDados, separador);
	while(token != NULL){
	if(i >= numAtributos){
		printf("Erro na linha de dados: %s\n", linhaDados);
		fprintf(stderr, "Número de dados (maior) não confere com o número de atributos.\n");
		exit(1);
	}
	strtrim(token);
	dados[i] = strdup(token);
	if(!dados[i]){
		fprintf(stderr, "Falha ao alocar memória para os dados ao duplicar o token\n");
		exit(1);
	}
	token = strtok(NULL, separador);
	i++;
	}
	if(i != numAtributos){
		printf("Erro na linha de dados: %s\n", linhaDados);
		fprintf(stderr, "Número de dados (menor) não confere com o número de atributos.\n");
		exit(1);
	}
	return dados;
}
// encontra a posição do atributo na lista de atributos
unsigned int verificaPosicaoAtributo(atributo* listaDeAtributos, int numAtributos, char* atributo){
	for(unsigned int i = 0; i < numAtributos; i++){
		if(strcmp(listaDeAtributos[i].rotulo, atributo) == 0){
			return i;
		}
	}
	printf("Atributo %s não encontrado, fechando a execução\n", atributo);
	fprintf(stderr, "Atributo não encontrado ao verificar a posição deste.\n");
	exit(1);
}
// conta quantas categorias um atributo do tipo categórico tem
unsigned int contaCategorias(char** categorias){
	unsigned int i = 0;
	char** copiaCategorias = categorias;
	while(copiaCategorias[i] != NULL){
		i++;
	}
	return i;
}
// mostra no stdout os atributos do arquivo
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
// conta o número de atributos do arquivo
int conta_atributos(FILE *arff){
//Fun��o do A1
	char percorre[TAM_LIN_ATR+1];
	char* token = NULL; // mudança dia 21/09 esse null
	char separador[] = " "; // para separar os atributos/tipos
	int numAtributos = 0;

	while(!feof(arff)){
		fgets(percorre, TAM_LIN_ATR, arff);
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
// separa as categorias do atributo no vetor de categorias dele
void processa_categorias(atributo *elemento, char *categorias){
//Recbe uma string com as categorias e atualiza o elemento com um vetor de strings (modificar a struct)
	char* token = NULL;
	char* percorre = categorias;
	unsigned int tamanhoFinal = 0; 
	int numCategorias = 1;

	token = strtok(percorre, ",");
	token = token + 1; //para ignorar o '{'
	elemento->categorias = malloc(sizeof(char*) * numCategorias);
	if(!elemento->categorias){
		fprintf(stderr, "Falha ao alocar memória para as categorias\n");
		exit(1);
	}
	elemento->categorias[0] = strdup(token);
	if(!elemento->categorias[0]){
		fprintf(stderr, "Falha ao alocar memória ao duplicar o token para as categorias\n");
		exit(1);
	}
	while(*percorre != '}'){
		if(*percorre == '\n'){
			fprintf(stderr, "Arquivo inválido devido a uma categoria não ser fechada.\n");
			exit(1);
		}
		if(*percorre == ',' || *(percorre+1) == '}'){
		token = strtok(NULL, ",");
		numCategorias++;
		elemento->categorias = realloc(elemento->categorias, sizeof(char*) * (numCategorias+1));
		if(!elemento->categorias){
			fprintf(stderr, "Falha ao re-alocar memória para as categorias\n");
			exit(1);
		}
		elemento->categorias[numCategorias-1] = strdup(token);
		if(!elemento->categorias[numCategorias-1]){
			fprintf(stderr, "Falha ao alocar memória ao duplicar o token para as categorias\n");
			exit(1);
		}
		}
		percorre++;
	}
	tamanhoFinal = strlen(elemento->categorias[numCategorias-1]);
	elemento->categorias[numCategorias-1][tamanhoFinal-2] = '\0'; // para remover o '\n' do final
	elemento->categorias[numCategorias] = NULL; // para indicar o final
}
// processa os atributos do arquivo, validando cada um deles e retornando um vetor de atributos
atributo* processa_atributos(FILE *arff, int quantidade){
//Fun��o do A1 (com modifica��es para o atributo de categorias)
	atributo* atributosArff;
	char percorre[TAM_LIN_ATR+1];
	char percorreCopia[TAM_LIN_ATR+1];
	char separador[] = " "; // para separar os atributos/tipos
	char* token;
	char* tokenCopia;
	int numAtributos = conta_atributos(arff);
	if(numAtributos == 0){
	fprintf(stderr, "Arquivo inválido devido a não ter atributos.\n");
	exit(1);
	}
	int i = 0;
	
	atributosArff = malloc(sizeof(atributo) * numAtributos);
	if(!atributosArff){
	fprintf(stderr, "falha ao alocar memória\n");
	exit(1);
	}

	while(!feof(arff)){
		fgets(percorre, TAM_LIN_ATR, arff);
		strcpy(percorreCopia, percorre);
		token = strtok(percorre, separador);
		if(strcmp("@attribute", token) == 0){
			//rotulo
			token = strtok(NULL, separador);
			atributosArff[i].rotulo = strdup(token);
			if(!atributosArff[i].rotulo){
			fprintf(stderr, "Falha ao alocar memória ao duplicar o token para o rótulo.\n");
			exit(1);
			}
			//tipo
			token = strtok(NULL, separador);
			if(!token){
				printf("Linha apresentando problema: %s\n", percorreCopia);
				fprintf(stderr, "Atributo inválido, faltando elementos da linha.\n");
				exit(1);
			}
			if((strcmp("numeric\n", token) == 0 )||(strcmp(token, "string\n") == 0)){
				atributosArff[i].tipo = strdup(token);
				if(!atributosArff[i].tipo){
					fprintf(stderr, "Falha ao alocar memória ao duplicar o token para o tipo.\n");
					exit(1);
				}
				atributosArff[i].categorias = NULL;
			}else{
				if(token[0] == '{'){ 
				//categorias
				atributosArff[i].tipo = strdup("categoric");
				if(!atributosArff[i].tipo){
					fprintf(stderr, "Falha ao alocar memória ao duplicar o token para o tipo categoric.\n");
					exit(1);
				}
				tokenCopia = strdup(token);
				if(!tokenCopia){
					fprintf(stderr, "Falha ao alocar memória ao duplicar o token para o processaCategorias.\n");
					exit(1);
				}
				processa_categorias(&atributosArff[i], tokenCopia); 
				free(tokenCopia);
				}else{
				printf("Atributo %s não é válido, fechando a execução.\n", token);
				fprintf(stderr,"Atributo inválido\n");
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
			fprintf(stderr, "Atributo inválido\n");
			exit(1);
		}
		}
	}
	return 0;
}
// simliar a contaAtributos, porém não retorna o arquivo pro  começo
int percorreAtributos(FILE *arff){
	char percorre[TAM_LIN_ATR+1];
	char* token = NULL; // mudança dia 21/09 esse null
	char separador[] = " "; // para separar os atributos/tipos
	int numAtributos = 0;

	while(!feof(arff)){
		fgets(percorre, TAM_LIN_ATR, arff);
		token = strtok(percorre, separador);
		strtrim(token);
		if(strcmp("@attribute", token) == 0){
			numAtributos += 1;        
		}
		if(strcmp("@data", token) == 0){
			return numAtributos;
		}
	}
	return 0;
}  
// valida todo o arquivo arff, desde os atributos até os dados
void valida_arff(FILE *arff, atributo *atributos, int quantidade){
//Recebe um arquivo arff com ponteiro de leitura antes do "@data"; passa por todas as linhas de dados e valida cada elementos de cada coluna em
//rela��o ao vetor de atributos tamb�m fornecido como argumento.
if(quantidade == 0){
	fprintf(stderr, "Arquivo inválido devido a não ter atributos.\n");
	exit(1);
}
char* linhaDados;
char** dados = NULL;
int numeroDeCategorias = 0;
int j = 0;

if(percorreAtributos(arff) == 0){
	fprintf(stderr, "Arquivo inválido devido a invalidez da sessão de atributos.\n");
	exit(1);
}
while(1){
	linhaDados = coletaLinhaDeDados(arff);
	if(!linhaDados)
		break; 
	dados = separarDados(linhaDados, quantidade);
	for(unsigned int i = 0; i < quantidade; i++){
	if(strcmp(atributos[i].tipo, "numeric\n") == 0 && atol(dados[i]) == 0 && strcmp(dados[i], "0") != 0){
		if(atof(dados[i]) == 0){
		printf("Arquivo inválido devido ao dado do atributo %s não ser numérico.\n", atributos[i].rotulo);
		fprintf(stderr, "Arquivo inválido devido a um valor não numérico.\n");
		exit(1);
		}
	}else if(strcmp(atributos[i].tipo, "string\n") == 0 && dados[i] == NULL){
		printf("Arquivo inválido devido ao dado do atributo %s não ser atribuído.\n", atributos[i].rotulo);
		fprintf(stderr, "Arquivo inválido devido a um valor nulo onde era esperado uam string.\n");
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
		fprintf(stderr, "Arquivo inválido devido a um valor não ser uma categoria válida.\n");
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