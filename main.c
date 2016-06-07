/***
	*
	*	--- RPG IP 2016 ---
	* @descricao main.c é o arquivo principal do jogo, onde toda a mágica acontece!
	* @desde 06/06/2016
	* @autores
	*   [
	*	 	  Bruno Melo
	* 		Filipe Cumaru
	*		  Rafael Santana
	*		  Victor Aurélio
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
	int ataque, defesa, vida;// Variáveis que representam quanto de ataque, defesa ou vida o personagem possui
	int x, y;// Coordenadas X e Y do personagem no mapa
}Personagem;

typedef struct{
	int tamanho; // Tamanho de  Linhas x Colunas (Nossa matriz será sempre quadrada)
	char **mapa; // A matriz que representa o mapa
	Personagem *heroi;
	Personagem *monstros;
}Jogo;

/***
	*
	*	Declarando cabeçalhos das funções
	*
***/
char pegarOpcaoMenu();
void exibirMenu(char *arquivoDeMenu, char *ultimo, char *opcao);
void exibirArquivo(char *nomeDoArquivo);
void iniciarJogo(Jogo *atual);
void carregarJogo(Jogo *atual);
void criarJogo(Jogo *atual);
void carregarPersonagensSalvos();
int buscarNomeEmArquivo(char *busca, char *nomeArquivo);
void alterarCaracterPrePalavra(char *texto, char *busca, char c);
void carregarMapaSalvoEmArquivo(Jogo *jogo, char *nomeArquivoMapa);
void carregarPersonagensSalvosEmArquivo(char *nomeDoArquivo, Jogo *jogo);
void salvarMapaEmMapasSalvos(char *nomeDoMapa);
void salvarPersonagemEmPersonagensSalvos(char *nomeDoPersonagem);
void criarNovoPacoteDePersonagens(Jogo *jogo);
void criarPacotePersonagens(char *nomePacotePersonagens, Personagem *heroi, Personagem *monstros);

int main(int argc, char const *argv[]){
	Jogo *jogoAtual = (Jogo*) malloc(sizeof(Jogo));
	jogoAtual->heroi = (Personagem*) malloc(sizeof(Personagem));
	jogoAtual->monstros = (Personagem*) malloc(sizeof(Personagem) * 4);


	while(1){
		switch(pegarOpcaoMenu("templates/tela_inicial")){// De acordo com a opção que o menu retornar
			case 'W': //Caso seja selecionada a primeira opção do menu
				iniciarJogo(jogoAtual);
				break;
			case 'S': //Caso seja selecionada a opção de baixo do menu (GAME OPTIONS)
				switch(pegarOpcaoMenu("templates/game_options")){
					case 'W': // CONFIGURAÇÕES DOS MAPAS
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
					case 'S': // CONFIGURAÇÕES DE PERSONAGENS
						switch(pegarOpcaoMenu("templates/characters_options")){
							case 'W': //SELECIONAR UM PERSONAGEM
								carregarPersonagensSalvos();
							break;
							case 'S': //CRIAR UM PERSONAGEM
								criarNovoPacoteDePersonagens(jogoAtual);
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
	*	Esta retorna qual a opção escolhida pelo usuário no menu.
	*
***/
char pegarOpcaoMenu(char *nomeArquivoDeMenu){
	char ultimo='S', opcao='W';
	do{
		if(ultimo != opcao && (opcao == 'W' || opcao == 'S')){// Só vai redezenhar se ele realmente alternar entre as posições...
			system("CLS");
			exibirMenu(nomeArquivoDeMenu, &ultimo, &opcao);
		}
	}while((opcao = toupper(getch())) && opcao != 13 && opcao != 3 && opcao != 'H');
	// OPCAO == 3 CTRL +c
	// OPCAO == 13 ENTER

	//Casos especiais, após o fim do menu, para sair do jogo ou para voltar a tela principal.
	if(opcao == 3 && (strcmp(nomeArquivoDeMenu,"templates/tela_inicial") == 0 || strcmp(nomeArquivoDeMenu, "templates/game_over") == 0)){ //Ctrl+C pressionado
		printf("Jogo finalizado!\n");
		exit(1);
	}else if(opcao == 3){
		return 0;//vai retornar um case inválido (diferente de w, e de s).. fará com que retorne para o menu principal.
	}else if(opcao == 'H'){
		ultimo = 'H';
	}
	return ultimo;
}
/***
	*
	*	Esta função exibe o menu e de acordo com a atual posição do cursor
	*   ela coloca '>' em cima ou em baixo do texto.
	*
***/
void exibirMenu(char *nomeArquivoDeMenu, char *ultimo, char *opcao){
	int i;
	char texto[300], palavras[2][50];
	FILE *file = fopen(nomeArquivoDeMenu, "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para não encontrado, só funciona no windows. **/
		#endif
		printf("\n\tArquivo %s nao encontrado!\n", nomeArquivoDeMenu);
		exit(1);
	}else{
		for(i=0; (!feof(file)); i++){
			if(i==0){
				//ANTES DE TUDO, vai escanear apenas as duas primeiras palavras do Arquivo!!
				// PALAVRA 0 E PALAVRA 1 serão as palavras que representam as opções disponiveis no menu.
				fscanf((FILE*) file, " %s", palavras[0]);
				fscanf((FILE*) file, " %s", palavras[1]);
			}else{
				// As próximas linhas de leitura
				//Temos as duas primeiras palavras escaneadas, sabemos diretamente do menu quem deve ser a opção de cima e a opção de baixo.
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
	*	Esta função recebe um texto como parametro,
	* 	busca uma determinada palavra e coloca um caracter duas posições antes da palavra.
	*
***/
void alterarCaracterPrePalavra(char *texto, char *busca, char c){
	char *ptr, *aux;
	aux = strstr(texto,busca); // Procura por uma palavra no texto e retorna o endereço da posição do caracter 0 desta palavra.
	if(aux != NULL){
		ptr = aux-2; // Volta 2 caracteres antes do caracter de inicio da palavra.
		*ptr = c;// Substitui o conteúdo pelo caracter C que foi passado como parametro
	}
}
/***
	*
	*	Esta função irá iniciar um jogo novo.
	*
***/
void iniciarJogo(Jogo *atual){
}
/***
	*
	*	Esta função irá criar um novo jogo, ou seja, novos mapas e personagens
	*
***/
void criarJogo(Jogo *atual){
}
/***
	*
	* 	Essa função carrega os mapas salvos. Apartir do arquivo mapas.txt
	*
***/
int carregarMapasSalvos(){
	int i,mapaSelecionado;
	char texto[300];
	FILE *file = fopen("mapas.txt", "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para não encontrado, só funciona no windows. **/
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
	* 	Essa função carrega os mapas salvos. Apartir do arquivo mapas.txt
	*
***/
void salvarMapaEmMapasSalvos(char *nomeDoMapa){
	FILE *file = fopen("mapas.txt", "a");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para não encontrado, só funciona no windows. **/
		#endif
		printf("\n\tArquivo de mapas nao encontrado!\n");
		exit(1);
	}else{
		system("CLS");
		//fputs("\n", (FILE*) file);
		fputs(nomeDoMapa, (FILE*) file);
	}
	fclose(file);
}
/***
	*
	* 	Essa função carrega o mapa do arquivo de texto.
	*
***/
void carregarMapaSalvoEmArquivo(Jogo *jogo, char *nomeArquivoMapa){
	int i,nLinhas=1;
	char texto[300];
	FILE *file = fopen(nomeArquivoMapa, "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para não encontrado, só funciona no windows. **/
		#endif
		printf("\n\tArquivo mapa nao encontrado!\n");
		exit(1);
	}else{
		fscanf((FILE*) file, "%d", &nLinhas);
		//printf("Numero de linhas: %d\n", nLinhas);
		jogo->mapa = (char**) malloc(sizeof(char*) * nLinhas);
		//para ignoranr o \n que vem após o número..
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
	* 	Essa função gerencia a criação de um novo pacote de personagens
	*
***/
void criarNovoPacoteDePersonagens(Jogo *jogo){
		int i;
		char nomePacotePersonagens[50];
		system("CLS");
		printf("Bem vindo a criacao de um novo pacote de personagens!!\n");
		printf("Digite o nome do pacote de personagens(ate 50 caracteres, sem espacos): \n");
		scanf(" %50s", &nomePacotePersonagens);
		while(buscarNomeEmArquivo(nomePacotePersonagens, "personagens.txt") > 0){
			printf("Ja existe um pacote de personagens com este nome, por favor, digite outro: \n");
			scanf(" %50s", &nomePacotePersonagens);
		}
		printf("Digite o nome do heroi\n");
		scanf(" %[^\n]",jogo->heroi->nome);
		printf("Digite a quantidade de vida que o heroi possui: \n");
		scanf("%d", &jogo->heroi->vida);
		printf("Digite a quantidade de ataque que o heroi possui: \n");
		scanf("%d", &jogo->heroi->ataque);
		printf("Digite a quantidade de defesa que o heroi possui: \n");
		scanf("%d", &jogo->heroi->defesa);
		for(i=0; i<4; i++){
			printf("Digite o nome do monstro %d\n", i+1);
			scanf(" %[^\n]",jogo->monstros[i].nome);
			printf("Digite a quantidade de vida que o heroi possui: \n");
			scanf("%d", &jogo->monstros[i].vida);
			printf("Digite a quantidade de ataque que o heroi possui: \n");
			scanf("%d", &jogo->monstros[i].ataque);
			printf("Digite a quantidade de defesa que o heroi possui: \n");
			scanf("%d", &jogo->monstros[i].defesa);
		}
		salvarPersonagemEmPersonagensSalvos(nomePacotePersonagens);
		criarPacotePersonagens(nomePacotePersonagens, jogo->heroi, jogo->monstros);
}
/***
	*
	* 	Essa função salva o nome do pacote de personagens em personagens salvos.
	*
***/
void salvarPersonagemEmPersonagensSalvos(char *nomeDoPersonagem){
	FILE *file = fopen("personagens.txt", "a");
	if(file == NULL){
		printf("\n\tArquivo de personagens nao encontrado!\n");
		exit(1);
	}else{
		fprintf((FILE*) file, "\n");
		fputs(nomeDoPersonagem, (FILE*) file);
	}
	fclose(file);
}
/***
	*
	* 	Essa função salva o nome do pacote de personagens em personagens salvos.
	*
***/
void criarPacotePersonagens(char *nomePacotePersonagens, Personagem *heroi, Personagem *monstros){
		char aux[64];
		int i;
		strcpy(aux, "personagens/");
		strcat(aux, nomePacotePersonagens);
		strcat(aux, ".bin");
		FILE *file = fopen(aux, "wb");
		if(file == NULL){
				printf("\n\tNão foi possível criar um pacote de personagens!\n");
				exit(1);
		}else{
				fwrite(heroi, sizeof(Personagem), 1, file);
				for(i=0; i<4; i++)
						fwrite(&monstros[i], sizeof(Personagem), 1, file);
		}
		fclose(file);
}
/***
	*
	* 	Essa função carrega os personagens do arquivo binário.
	*
***/
void carregarPersonagensSalvos(){
	int i,personagemSelecionado;
	char texto[50];
	FILE *file = fopen("personagens.txt", "r");
	if(file == NULL){
		printf("\n\tArquivo de personagens nao encontrado!\n");
		exit(1);
	}else{
		system("CLS");
		printf("\n\nPersonagens encontrados na base de dados:\nCod:\tNome:\n");
		for(i=0; (!feof(file)); i++){
			fgets(texto, 50, (FILE*) file);
			printf("%d\t %s", i+1,texto);
		}
		printf("\nPressione o codigo do pacote de personagens que deseja usar: ");
		scanf("%d", &personagemSelecionado);
	}
}
/***
	*
	* 	Essa função carrega os personagens do arquivo binário.
	*
***/
void carregarPersonagensSalvosEmArquivo(char *nomeDoArquivo, Jogo *jogo){
	FILE *file = fopen("binario.bin", "rb");
	fread(jogo->heroi, sizeof(Personagem), 1, file);
	int i;
	for(i=0; i<4; i++){
		fread(&jogo->monstros[i], sizeof(Personagem), 1, file);
	}
	printf("%s\n", jogo->heroi->nome);
	fclose(file);
}
/***
	*
	* 	Essa função pesquisa por um nome de até 50 caracteres em algum arquivo que armazene strings de até 50 caracteres, linha por linha..
	*
***/
int buscarNomeEmArquivo(char *busca, char *nomeArquivo){
	FILE *file = fopen(nomeArquivo, "r");
	if(file == NULL){
			printf("\n\nTentando buscar a palavra %s no arquivo %s mas o arquivo não foi encontrado!\n", busca, nomeArquivo);
	}else{
			char texto[50];
			while(!feof(file)){
					fgets(texto,50, (FILE*) file);
					if(strcmp(texto, busca) == 0){
							return 1;
					}
			}
	}
	return 0;
}
/***
	*
	* 	Essa função carrega um arquivo de texto e exibe-o na tela.
	*
***/
void exibirArquivo(char *nomeDoArquivo){
	FILE *file;
	file = fopen(nomeDoArquivo, "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
				printf("\a\a");/** Dois beep's para não encontrado, só funciona no windows. **/
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
			printf("\a\a");/** Dois beep's para não encontrado, só funciona no windows. **/
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
