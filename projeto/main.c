/***
	*
	*	--- RPG IP 2016 ---
	* @descricao main.c � o arquivo principal do jogo, onde toda a m�gica acontece!
	* @desde 06/06/2016
	* @autores
	*   [
	*	 	  Bruno Melo
	* 		  Filipe Cumaru
	*		  Rafael Santana - @arbs
	*		  Victor Aur�lio
	* 	]
	*
	*
***/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>

#define CLS cls;
#if UNIX //Se estiver em um sistema linux
	#define CLS clear
#endif

typedef struct{
	char representacao;// A letra que vai representar o personagem no mapa
	char nome[20]; //O Nome do personagem
	int ataque, defesa, vida;// Vari�veis que representam quanto de ataque, defesa ou vida o personagem possui
	int x, y;// Coordenadas X e Y do personagem no mapa
}Personagem;

typedef struct{
	int tamanho; // Tamanho de  Linhas x Colunas (Nossa matriz ser� sempre quadrada)
	char **mapa; // A matriz que representa o mapa
}Jogo;

/***
	*
	*	Declarando cabe�alhos das fun��es
	*
***/
char pegarOpcaoMenu();
void exibirMenu(char *arquivoDeMenu, char *ultimo, char *opcao);
void exibirArquivo(char *nomeDoArquivo);
void iniciarJogo(Personagem *heroi, Personagem *monstros, Jogo *atual);
void carregarJogo(Personagem *heroi, Personagem *monstros, Jogo *atual);
void criarJogo(Personagem *heroi, Personagem *monstros, Jogo *atual);
void alterarCaracterPrePalavra(char *texto, char *busca, char c);
void carregarMapaSalvoEmArquivo(Jogo *jogo, char *nomeArquivoMapa);
void carregarPersonagensSalvosEmArquivo(Personagem *heroi, Personagem *monstros);
void salvarMapaEmMapasSalvos(char *nomeDoMapa);

int main(int argc, char const *argv[]){
	Personagem *heroi = (Personagem*) malloc(sizeof(Personagem));
	Personagem *monstros = (Personagem*) malloc(sizeof(Personagem) * 4);
	Jogo *jogoAtual = (Jogo*) malloc(sizeof(Jogo));

	while(1){
		switch(pegarOpcaoMenu("templates/tela_inicial")){// De acordo com a op��o que o menu retornar
			case 'W': //Caso seja selecionada a primeira op��o do menu
				iniciarJogo(heroi, monstros, jogoAtual);
				break;
			case 'S': //Caso seja selecionada a op��o de baixo do menu (GAME OPTIONS)
				switch(pegarOpcaoMenu("templates/game_options")){
					case 'W': // CONFIGURA��ES DOS MAPAS
						switch(pegarOpcaoMenu("templates/map_options")){
							case 'W': //SELECIONAR UM MAPA
								carregarMapasSalvos();
							break;
							case 'S': //CRIAR UM MAPA
								system("cls");
								char nome[200];
								printf("digite o nome do mapa: ");
								gets(nome);
								salvarMapaEmMapasSalvos(nome);
							break;
						};
						break;
					case 'S': // CONFIGURA��ES DE PERSONAGENS
						switch(pegarOpcaoMenu("templates/characters_options")){
							case 'W': //SELECIONAR UM MAPA
							break;
							case 'S': //CRIAR UM MAPA
							break;
						};
				}
				break;
			case 'H':
				system("CLS");
				system("color 0e");
				exibirArquivo("templates/help");
				system("pause");
		};
	}
	return 0;
}
/***
	*
	*	Esta retorna qual a op��o escolhida pelo usu�rio no menu.
	*
***/
char pegarOpcaoMenu(char *nomeArquivoDeMenu){
	char ultimo='S', opcao='W';
	do{
		if(ultimo != opcao && (opcao == 'W' || opcao == 'S')){// S� vai redezenhar se ele realmente alternar entre as posi��es...
			system("CLS");
			exibirMenu(nomeArquivoDeMenu, &ultimo, &opcao);
		}
	}while((opcao = toupper(getch())) && opcao != 13 && opcao != 3 && opcao != 'H');
	// OPCAO == 3 CTRL +c
	// OPCAO == 13 ENTER

	//Casos especiais, ap�s o fim do menu, para sair do jogo ou para voltar a tela principal.
	if(opcao == 3 && (strcmp(nomeArquivoDeMenu,"templates/tela_inicial") == 0 || strcmp(nomeArquivoDeMenu, "templates/game_over") == 0)){ //Ctrl+C pressionado
		printf("Jogo finalizado!\n");
		exit(1);
	}else if(opcao == 3){
		return 0;//vai retornar um case inv�lido (diferente de w, e de s).. far� com que retorne para o menu principal.
	}else if(opcao == 'H'){
		ultimo = 'H';
	}
	return ultimo;
}
/***
	*
	*	Esta fun��o exibe o menu e de acordo com a atual posi��o do cursor
	*   ela coloca '>' em cima ou em baixo do texto.
	*
***/
void exibirMenu(char *nomeArquivoDeMenu, char *ultimo, char *opcao){
	int i;
	char texto[300], palavras[2][50];
	FILE *file = fopen(nomeArquivoDeMenu, "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para n�o encontrado, s� funciona no windows. **/
		#endif
		printf("\n\tArquivo %s nao encontrado!\n", nomeArquivoDeMenu);
		exit(1);
	}else{
		for(i=0; (!feof(file)); i++){
			if(i==0){
				//ANTES DE TUDO, vai escanear apenas as duas primeiras palavras do Arquivo!!
				// PALAVRA 0 E PALAVRA 1 ser�o as palavras que representam as op��es disponiveis no menu.
				fscanf((FILE*) file, " %s", palavras[0]);
				fscanf((FILE*) file, " %s", palavras[1]);
			}else{
				// As pr�ximas linhas de leitura
				//Temos as duas primeiras palavras escaneadas, sabemos diretamente do menu quem deve ser a op��o de cima e a op��o de baixo.
				fgets(texto, 300, (FILE*) file);
				if(*opcao == 'W'){
					*ultimo = 'W';
					alterarCaracterPrePalavra(texto, palavras[0], '>');
					alterarCaracterPrePalavra(texto, palavras[1], ' ');
				}else if(*opcao == 'S'){
					*ultimo = 'S';
					alterarCaracterPrePalavra(texto, palavras[0], ' ');
					alterarCaracterPrePalavra(texto, palavras[1], '>');
				}
				printf("%s", texto);
			}
		}
	}
	fclose(file);
}
/***
	*
	*	Esta fun��o recebe um texto como parametro,
	* 	busca uma determinada palavra e coloca um caracter duas posi��es antes da palavra.
	*
***/
void alterarCaracterPrePalavra(char *texto, char *busca, char c){
	char *ptr, *aux;
	aux = strstr(texto,busca); // Procura por uma palavra no texto e retorna o endere�o da posi��o do caracter 0 desta palavra.
	if(aux != NULL){
		ptr = aux-2; // Volta 2 caracteres antes do caracter de inicio da palavra.
		*ptr = c;// Substitui o conte�do pelo caracter C que foi passado como parametro
	}
}
/***
	*
	*	Esta fun��o ir� iniciar um jogo novo.
	*
***/
void iniciarJogo(Personagem *heroi, Personagem *monstros, Jogo *atual){
}
/***
	*
	*	Esta fun��o ir� criar um novo jogo, ou seja, novos mapas e personagens
	*
***/
void criarJogo(Personagem *heroi, Personagem *monstros, Jogo *atual){
}
/***
	*
	* 	Essa fun��o carrega os mapas salvos. Apartir do arquivo mapas.txt
	*
***/
int carregarMapasSalvos(){
	int i,mapaSelecionado;
	char texto[300];
	FILE *file = fopen("mapas.txt", "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para n�o encontrado, s� funciona no windows. **/
		#endif
		printf("\n\tArquivo de mapas nao encontrado!\n");
		exit(1);
	}else{
		system("CLS");
		printf("\n\nMapas encontrados na base de dados:\nCod:\tNome:\n");
		for(i=0; (!feof(file)); i++){
			fgets(texto, 300, (FILE*) file);
			printf("%d\t %s", i+1,texto);
		}
		printf("\nPressione o codigo do mapa que deseja usar: ");
		scanf("%d", &mapaSelecionado);
		return mapaSelecionado-1;
	}
}
/***
	*
	* 	Essa fun��o carrega os mapas salvos. Apartir do arquivo mapas.txt
	*
***/
void salvarMapaEmMapasSalvos(char *nomeDoMapa){
	FILE *file = fopen("mapas.txt", "a");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para n�o encontrado, s� funciona no windows. **/
		#endif
		printf("\n\tArquivo de mapas nao encontrado!\n");
		exit(1);
	}else{
		system("CLS");
		//fputs("\n", (FILE*) file);
		fseek (file , 0 , SEEK_END);
		fputs("\n",(FILE*) file);
		fputs(nomeDoMapa, (FILE*) file);
	}
	fclose(file);
}
/***
	*
	* 	Essa fun��o carrega o mapa do arquivo de texto.
	*
***/
void carregarMapaSalvoEmArquivo(Jogo *jogo, char *nomeArquivoMapa){
	int i,nLinhas=1;
	char texto[300];
	FILE *file = fopen(nomeArquivoMapa, "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para n�o encontrado, s� funciona no windows. **/
		#endif
		printf("\n\tArquivo mapa nao encontrado!\n");
		exit(1);
	}else{
		fscanf((FILE*) file, "%d", &nLinhas);
		//printf("Numero de linhas: %d\n", nLinhas);
		jogo->mapa = (char**) malloc(sizeof(char*) * nLinhas);
		//para ignoranr o \n que vem ap�s o n�mero..
		jogo->mapa[0] = (char*) malloc(sizeof(char) * 300);
		fgets(jogo->mapa[0], 300, (FILE*) file);
		for(i=0; i<nLinhas; i++){
			jogo->mapa[i] = (char*) malloc(sizeof(char) * 300);
			fgets(jogo->mapa[i], 300, (FILE*) file);
		}
		/*for(i=0; i<nLinhas; i++){
			printf("| %s",jogo->mapa[i]);
		}*/
	}
}
/***
	*
	* 	Essa fun��o carrega os personagens do arquivo bin�rio.
	*
***/
void carregarPersonagensSalvosEmArquivo(Personagem *heroi, Personagem *monstros){
	FILE *file = fopen("binario.bin", "rb");
	fread(heroi, sizeof(Personagem), 1, file);
	printf("%s\n", heroi->nome);
}
/***
	*
	* 	Essa fun��o carrega um arquivo de texto e exibe-o na tela.
	*
***/
void exibirArquivo(char *nomeDoArquivo){
	FILE *file;
	file = fopen(nomeDoArquivo, "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
				printf("\a\a");/** Dois beep's para n�o encontrado, s� funciona no windows. **/
				#endif
		printf("\n\tArquivo %s nao encontrado!\n", nomeDoArquivo);
		exit(1);
	}else{
			char texto[300];
			while(!feof(file)){
				fgets(texto, 300, (FILE*) file);
				printf("%s", texto);
			}
	}
	fclose(file);
}
/***
		*
		*
		*
***/
int numeroDeLinhasArquivoTexto(char *nomeArquivo){
	FILE *file = fopen(nomeArquivo, "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para n�o encontrado, s� funciona no windows. **/
		#endif
		printf("\n\tArquivo de mapas nao encontrado!\n");
		exit(1);
	}else{
		system("CLS");
		int i=0;
		char texto[300];
		for(i=0; (!feof(file)); i++){
			fgets(texto, 300, (FILE*) file);
		}
		return i;
	}
	return -1;
}
