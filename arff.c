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
		memmove(linha, linha + comeco, final - comeco + 1); // +1 para copiar todos os caracteres entre o intervalo   
		linha[final - comeco + 1] = '\0'; // para indicar o final da string
	}
}
// coleta a linha de dados com o tamanho máximo de TAM_LIN_DADOS            
char* coletaLinhaDeDados(FILE* arff){
	char percorre[TAM_LIN_DADOS+1]; // +1 para o '\0'

	if(fgets(percorre, TAM_LIN_DADOS, arff) == NULL){
		return NULL;
	} // se a linha for nula
	
	if(percorre[0] == '\n'){
		return coletaLinhaDeDados(arff);
	} // se a linha for vazia

	return strdup(percorre);
}
// separa os dados da linha com base no número de atributos
char** separarDados(char* linhaDados, int numAtributos){
	char* token = NULL;
	char separador[] = ",";
	char* copiaLinhaDados = strdup(linhaDados); // para mensagem de erro correta

	int i = 0;
	char** dados = (char**)malloc(sizeof(char*)*numAtributos);
	if(!dados){
		fprintf(stderr, "Falha ao alocar memória para os dados\n");
		exit(1);
	}

	token = strtok(linhaDados, separador);
	while(token != NULL){
	if(i >= numAtributos){ // se o número de dados for maior que o número de atributos, o arquivo é inválido
		fprintf(stderr, VRM "Erro na linha de dados:" VRD "%s\n" AML "Número de dados (maior) não confere com o número de atributos.\n" RESET, copiaLinhaDados);
		exit(1);
	}
	strtrim(token); // remove os '\n' do começo/final da string
	dados[i] = strdup(token);
	if(!dados[i]){
		fprintf(stderr, "Falha ao alocar memória para os dados ao duplicar o token\n");
		exit(1);
	}
	token = strtok(NULL, separador);
	i++;
	}
	if(i != numAtributos){ // se o número de dados for diferente de número de atributos, o arquivo é inválido
		fprintf(stderr, VRM "Erro na linha de dados:" VRD "%s\n" AML "Número de dados (menor) não confere com o número de atributos.\n" RESET, copiaLinhaDados);
		exit(1);
	}
	free(copiaLinhaDados);
	return dados;
}
// encontra a posição do atributo na lista de atributos
unsigned int verificaPosicaoAtributo(atributo* listaDeAtributos, int numAtributos, char* atributo){
	for(unsigned int i = 0; i < numAtributos; i++){
		if(strcmp(listaDeAtributos[i].rotulo, atributo) == 0){
			return i;
		}// se o atributo for encontrado retorna a posição dele no vetor de atributos
	}
	// se o atributo não for encontrado, o arquivo é inválido
	fprintf(stderr, VRM "Atributo " AML "%s" VRM " não encontrado, fechando a execução\n" RESET, atributo);
	exit(1);
}
// conta quantas categorias um atributo do tipo categórico tem
unsigned int contaCategorias(char** categorias){
	unsigned int i = 0;
	char** copiaCategorias = categorias;
	while(copiaCategorias[i] != NULL){
		i++;
	}// conta quantas categorias tem o atributo do tipo categórico
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
	char* token = NULL; 
	char separador[] = " "; 
	int numAtributos = 0;

	while(!feof(arff)){ // percorre o arquivo
		fgets(percorre, TAM_LIN_ATR, arff);
		token = strtok(percorre, separador);
		strtrim(token);
		if(strcmp("@attribute", token) == 0){ // valida a linha de atributo
			numAtributos += 1;        
		}
		if(strcmp("@data", token) == 0){ // valida o @data
			rewind(arff); // retorna o arquivo pro começo para validar os atributos em sua totalidade
			return numAtributos;
		}
	}
	return 0;
}
// separa as categorias do atributo no vetor de categorias dele
void processa_categorias(atributo *elemento, char *categorias){
//Recbe uma string com as categorias e atualiza o elemento com um vetor de strings (modificar a struct)
	char* token = NULL;
	char* percorre = categorias; // recebe a linha contendo as categorias
	unsigned int tamanhoFinal = 0; 
	int numCategorias = 1;

	token = strtok(percorre, ",");
	token = token + 1; //para ignorar o '{'
	elemento->categorias = malloc(sizeof(char*) * numCategorias);
	if(!elemento->categorias){
		fprintf(stderr, VRM "Falha ao alocar memória para as categorias\n" RESET);
		exit(1);
	}
	elemento->categorias[0] = strdup(token);
	if(!elemento->categorias[0]){
		fprintf(stderr, VRM "Falha ao alocar memória ao duplicar o token para as categorias\n" RESET);
		exit(1);
	}
	while(*percorre != '}'){ // enquanto não encontra o delimitador de categorias, continua o looping
		if(*percorre == '\n' || *percorre == EOF){ // se a linha acabar antes de encontrar o delimitador, o arquivo é inválido
			fprintf(stderr, VRM "Arquivo inválido devido a uma categoria não ser fechada.\n" AML "Categorias em questão: %s\n" RESET, categorias);
			exit(1);
		}
		if(*percorre == ',' || *(percorre+1) == '}'){ // se encontrar uma vírgula ou o delimitador, adiciona a categoria no vetor
		token = strtok(NULL, ",");
		numCategorias++;
		elemento->categorias = realloc(elemento->categorias, sizeof(char*) * (numCategorias+1)); // aloca +1 para ter o null como identificador de final
		if(!elemento->categorias){
			fprintf(stderr, VRM "Falha ao re-alocar memória para as categorias.\n" RESET);
			exit(1);
		}
		elemento->categorias[numCategorias-1] = strdup(token);
		if(!elemento->categorias[numCategorias-1]){
			fprintf(stderr, VRM "Falha ao alocar memória ao duplicar o token para as categorias.\n" RESET);
			exit(1);
		}
		}
		percorre++;
	}
	tamanhoFinal = strlen(elemento->categorias[numCategorias-1]); // mede o tamanho do último elemento
	elemento->categorias[numCategorias-1][tamanhoFinal-2] = '\0'; // para remover o '\n' do final e '}'
	elemento->categorias[numCategorias] = NULL; // para indicar o final
}
// processa os atributos do arquivo, validando cada um deles e retornando um vetor de atributos
atributo* processa_atributos(FILE *arff, int quantidade){
//Fun��o do A1 (com modifica��es para o atributo de categorias)
	atributo* atributosArff;
	char percorre[TAM_LIN_ATR+1];
	char percorreCopia[TAM_LIN_ATR+1]; // para copiar a linha e usar na mensagem de erro
	char separador[] = " "; // para separar os atributos/tipos
	char* token;
	char* tokenCopia;
	int numAtributos = conta_atributos(arff);
	if(numAtributos == 0){
	fprintf(stderr, VRM "Arquivo inválido devido a não ter atributos.\n" RESET);
	exit(1);
	}
	int i = 0;
	
	atributosArff = malloc(sizeof(atributo) * numAtributos);
	if(!atributosArff){
	fprintf(stderr, VRM "Falha ao alocar memória\n" RESET);
	exit(1);
	}

	while(!feof(arff)){
		fgets(percorre, TAM_LIN_ATR, arff);
		strcpy(percorreCopia, percorre); // copia a linha para não perder o ponteiro para a mensagem de erro
		token = strtok(percorre, separador);
		if(strcmp("@attribute", token) == 0){ // encontrou um atributo
			//rotulo
			token = strtok(NULL, separador); // pega o próximo elemento que é para ser o rótulo
			atributosArff[i].rotulo = strdup(token);
			if(!atributosArff[i].rotulo){
			fprintf(stderr, VRM "Falha ao alocar memória ao duplicar o token para o rótulo.\n" RESET);
			exit(1);
			}
			//tipo
			token = strtok(NULL, separador);
			if(!token){ // se faltar elementos, o arquivo é inválido
				fprintf(stderr, VRM "Atributo inválido, faltando elementos da linha.\nLinha apresentando problema:" AML " %s\n" RESET, percorreCopia);
				exit(1);
			}
			if((strcmp("numeric\n", token) == 0 )||(strcmp(token, "string\n") == 0)){ // verifica se é numeric ou string o tipo do atributo
				atributosArff[i].tipo = strdup(token);
				if(!atributosArff[i].tipo){
					fprintf(stderr, VRM "Falha ao alocar memória ao duplicar o token para o tipo.\n" RESET);
					exit(1);
				}
				atributosArff[i].categorias = NULL; // caso seja numeric ou string, não tem categorias
			}else{ // se não for numeric ou string, pode ser um atributo do tipo categórico
				if(token[0] == '{'){ // confirmação de que é um atributo do tipo categórico
				//categorias
				atributosArff[i].tipo = strdup("categoric"); // para indicar que é um atributo do tipo categórico
				if(!atributosArff[i].tipo){
					fprintf(stderr, VRM "Falha ao alocar memória ao duplicar o token para o tipo categoric.\n" RESET);
					exit(1);
				}
				tokenCopia = strdup(token); // preciso dessa cópia, para impedir vazamento de memória
				if(!tokenCopia){
					fprintf(stderr, VRM "Falha ao alocar memória ao duplicar o token para o processaCategorias.\n" RESET);
					exit(1);
				}
				processa_categorias(&atributosArff[i], tokenCopia); // separa as categorias em um vetor de strings
				free(tokenCopia);
				}else{
				fprintf(stderr, VRM "Atributo" AML " %s " VRM "não é válido, fechando a execução.\nDa linha" AML" %s\n" RESET, token, percorreCopia); // caso chegue aqui, o arquivo é inválido
				exit(1);
				}
			}
			i++;
		}else{
		strtrim(token); // remove os '\n' do começo/final da string
		if(strcmp("@data", token) == 0) // se encontrar o @data, retorna o vetor de atributos
			return atributosArff;
		else 
		if(percorre[0] != '\n'){ // se não for uma linha vazia, é uma linha de atributos que invalida o arquivo
			fprintf(stderr, VRM "Atributo" AML " %s " VRM "inválido\n" RESET, percorre);
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
if(quantidade == 0){ // caso não tenha atributos no arquivo, o arquivo é inválido
	fprintf(stderr, VRM "Arquivo inválido devido a não ter atributos.\n" RESET);
	exit(1);
}
char* linhaDados = NULL;
char* linhaDadosCopia = NULL; // para mensagem de erro
char** dados = NULL;
int numeroDeCategorias = 0;
int j = 0;

if(percorreAtributos(arff) == 0){
	fprintf(stderr, VRM "Arquivo inválido devido a invalidez da sessão de atributos.\n" RESET);
	exit(1);
}
while(1){
	linhaDados = coletaLinhaDeDados(arff);
	if(!linhaDados)
		break; 
	linhaDadosCopia = strdup(linhaDados); // copia a linha para não perder o ponteiro para a mensagem de erro
	if(!linhaDadosCopia){
		fprintf(stderr, VRM "Falha ao alocar memória ao duplicar a linha de dados.\n" RESET);
		exit(1);
	}
	dados = separarDados(linhaDados, quantidade);
	for(unsigned int i = 0; i < quantidade; i++){ // para cada dado, valida o tipo dele
	if(strcmp(atributos[i].tipo, "numeric\n") == 0 && atof(dados[i]) == 0 && strcmp(dados[i], "0") != 0 && strcmp(dados[i], "0.0")){ // se for numeric e não for um número nos dados, o arquivo pode ser inválido
		fprintf(stderr, VRM "Arquivo inválido devido ao dado do atributo" AML" %s " VRM "não ser numérico.\n" RESET, atributos[i].rotulo);
		fprintf(stderr, VRM "Dado inválido:" AML " %s\n" VRM "Linha em questão:" AML "%s\n" RESET, dados[i], linhaDadosCopia);
		exit(1);
	}else if(strcmp(atributos[i].tipo, "string\n") == 0 && dados[i] == NULL){ // se for string e for nulo nos dados, o arquivo é inválido
		fprintf(stderr, VRM "Arquivo inválido devido ao dado do atributo" AML " %s " VRM "não ser atribuído.\n" RESET, atributos[i].rotulo);
		fprintf(stderr, "Linha em questão: %s\n", linhaDadosCopia);
		exit(1);
	}else if(strcmp(atributos[i].tipo, "categoric") == 0){ // se for uma categoria
	numeroDeCategorias = contaCategorias(atributos[i].categorias); // conta quantas categorias tem o atributo
	while(numeroDeCategorias > 0){ // enquanto não chegar no final do vetor de categorias
		if(strcmp(dados[i], atributos[i].categorias[j]) == 0){ // verifica o dado em questão com todas as categorias
		break;
		}
		numeroDeCategorias--;
		j++;
	}
	if(numeroDeCategorias == 0){ // se não encontrar a categoria, o arquivo é inválido
		fprintf(stderr, VRM "Arquivo inválido devido ao dado do atributo" AML " %s " VRM "não ser uma categoria válida.\n" RESET, atributos[i].rotulo);
		fprintf(stderr, VRM "Dado inválido:" AML "%s\n" VRM "Linha em questão:" AML "%s\n" RESET, dados[i], linhaDadosCopia);
		exit(1);
	}
	j = 0; // reseta o j para o próximo loop
	}
} 
	for(int i = 0; i < quantidade; i++){
	free(dados[i]);
	}
	free(dados);
	free(linhaDados);
	free(linhaDadosCopia);
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