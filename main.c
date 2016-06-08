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
#include <string.h> // strcmp, stract, strcoy
#include <time.h>
#include <ctype.h> // toupper
#ifdef __unix__
	#include <curses.h>
	#define CLS "clear"
#elif _WIN32
	#define CLS "cls"
	#include <conio.h>
#endif


#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
  #include <windows.h>
  void delay( unsigned long ms ){
    Sleep( ms );
  }
#else  /* presume POSIX */
  #include <unistd.h>
  void delay( unsigned long ms ){
    usleep( ms * 1000 );//micro segundos
  }
#endif


typedef struct{
	char representacao;// A letra que vai representar o personagem no mapa
	char nome[20]; //O Nome do personagem
	int ataque, defesa, vida;// Variáveis que representam quanto de ataque, defesa ou vida o personagem possui
	int x, y;// Coordenadas X e Y do personagem no mapa
}Personagem;

typedef struct{
	int pontuacao;
	int tamanho; // Tamanho de  Linhas x Colunas (Nossa matriz será sempre quadrada)
	char *nomePacotePersonagens;
	char *nomeMapa;
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
void listarPersonagens();
void listarMapas();
int buscarNomeEmArquivo(char *busca, char *nomeArquivo);
void alterarCaracterPrePalavra(char *texto, char *busca, char c);
void carregarMapaParaOJogo(Jogo *jogo, char *nomeArquivoMapa);
void carregarPersonagensParaOJogo(char *nomeDoArquivo, Jogo *jogo);
void salvarMapaNoIndex(char *nomeDoMapa);
void salvarPersonagemNoIndex(char *nomeDoPersonagem);
void telaCriarPacoteDePersonagens(Jogo *jogo);
void criarPacotePersonagens(char *nomePacotePersonagens, Personagem *heroi, Personagem *monstros);
void criarMapa (char* name);
void nomeEmArquivoPorLinha(int numeroDaLinha, char *nomeDoArquivo);
void telaSelecionarMapa(Jogo *jogo);
void telaSelecionarPacoteDePersonagens(Jogo *jogo);
void telaCriarMapa();
void telaPreJogo(Jogo *jogo);
void desenharMapa(Jogo *jogo, int *rodada);

int main(int argc, char const *argv[]){
	Jogo *jogoAtual = (Jogo*) calloc(1,sizeof(Jogo));
	jogoAtual->heroi = (Personagem*) calloc(1,sizeof(Personagem));
	jogoAtual->monstros = (Personagem*) calloc(4,sizeof(Personagem));
	int exibirBemVindo=1;
	while(1){
		system(CLS);
		system("color 0f");
		if(exibirBemVindo){
			exibirArquivo("templates/Bem_Vindo");
			getch();
			exibirBemVindo = 0;
		}
		switch(pegarOpcaoMenu("templates/tela_inicial")){// De acordo com a opção que o menu retornar
			case 'W': //Caso seja selecionada a primeira opção do menu
				telaPreJogo(jogoAtual);
				break;
			case 'S': //Caso seja selecionada a opção de baixo do menu (GAME OPTIONS)
				switch(pegarOpcaoMenu("templates/game_options")){
					case 'W': // CONFIGURAÇÕES DOS MAPAS
						switch(pegarOpcaoMenu("templates/map_options")){
							case 'W': //SELECIONAR UM MAPA
								telaSelecionarMapa(jogoAtual);
							break;
							case 'S': //CRIAR UM MAPA
								telaCriarMapa(jogoAtual);
							break;
						};
						break;
					case 'S': // CONFIGURAÇÕES DE PERSONAGENS
						switch(pegarOpcaoMenu("templates/characters_options")){
							case 'W': //SELECIONAR UM PERSONAGEM
								telaSelecionarPacoteDePersonagens(jogoAtual);
							break;
							case 'S': //CRIAR UM PERSONAGEM
								telaCriarPacoteDePersonagens(jogoAtual);
							break;
						};
				}
				break;
			case 72:
				system(CLS);
				system("color 0e");
				exibirArquivo("templates/help");
				printf("\n");
				getch();
		};
	}
	exibirArquivo("templates/texto_final");
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
			system(CLS);
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
void telaPreJogo(Jogo *jogo){
		system(CLS);
		if(strcmp(jogo->heroi->nome, "") == 0){ // Heroi sempre vai estar alocado.. Porém se tivermos carregado um pacote de personagens teremos um nome para o heroi
				carregarPersonagensParaOJogo("default", jogo);
		}
		if(jogo->mapa == NULL){
				carregarMapaParaOJogo(jogo, "default");
		}
		if(jogo->heroi != NULL && jogo->mapa != NULL && jogo->monstros != NULL){
				exibirArquivo("templates/jogoCarregado");
				printf("\nMapa selecionado: %s",jogo->nomeMapa);
				printf("\nPacote de personagens selecionado: %s",jogo->nomePacotePersonagens);
				delay(2000); // Espera 2 segundos
				iniciarJogo(jogo);
		}else{
				printf("\nProblemas ao carregar o jogo!");
				exit(1);
		}
}
void iniciarJogo(Jogo *jogo){
	 	char teclaPressionada=0;
		int rodada=0, metadeTamanho = jogo->tamanho/2;
		jogo->heroi->x = metadeTamanho;//(3 * jogo->tamanho + 2)
		jogo->heroi->y = metadeTamanho;//(3 * jogo->tamanho + 2)
		jogo->monstros[0].x = metadeTamanho/2;
		jogo->monstros[0].y = metadeTamanho/2;
		jogo->monstros[1].x = metadeTamanho/2 + metadeTamanho;
		jogo->monstros[1].y = metadeTamanho/2;
		jogo->monstros[2].x = metadeTamanho/2;
		jogo->monstros[2].y = metadeTamanho/2 + metadeTamanho;
		jogo->monstros[3].x = metadeTamanho/2 + metadeTamanho;
		jogo->monstros[3].y = metadeTamanho/2 + metadeTamanho;
		system(CLS);
		desenharMapa(jogo, &rodada);
		while(jogo->monstros[0].vida > 0 && jogo->monstros[1].vida > 0  && jogo->monstros[2].vida > 0  && jogo->monstros[3].vida > 0 && teclaPressionada != 3){
			while(( teclaPressionada = toupper(getch()) ) && (teclaPressionada=='A' || teclaPressionada=='S' || teclaPressionada=='W' || teclaPressionada=='D') && teclaPressionada != 3){
				if(teclaPressionada!=0){
					switch (teclaPressionada) {
						case 'A':
								if(jogo->heroi->x > 1)
										jogo->heroi->x--;
						break;
						case 'W':
								if(jogo->heroi->y > 1)
										jogo->heroi->y--;
						break;
						case 'S':
								if(jogo->heroi->y < jogo->tamanho - 2)
										jogo->heroi->y++;
						break;
						case 'D':
								if(jogo->heroi->x < jogo->tamanho - 1)
										jogo->heroi->x++;
						break;
						default:break;
					};
					if(jogo->mapa[jogo->heroi->y][jogo->heroi->x] == '*'){
						jogo->mapa[jogo->heroi->y][jogo->heroi->x] = ' ';
						jogo->heroi->vida+=20;
					}
					//movimentarMonstros();
				}
				rodada++;
				system(CLS);
				desenharMapa(jogo, &rodada);
			}
		}
}
void desenharMapa(Jogo *jogo, int *rodada){
	int i,j,k,imprimirConteudoDoMapa;
	for(i=0; i<jogo->tamanho; i++){
		for(j=0; j<=jogo->tamanho; j++){
				imprimirConteudoDoMapa=1;
				if(jogo->heroi->x == j && jogo->heroi->y == i){
						printf("H");
						imprimirConteudoDoMapa=0;
				}
				for(k=0; k<4; k++){
						if(jogo->monstros[k].x == j && jogo->monstros[k].y == i){
							printf("M");//representacao alerta!!!
							imprimirConteudoDoMapa=0;
						}
				}
				if(imprimirConteudoDoMapa){
						printf("%c",jogo->mapa[i][j]);
				}
		}
		printf("\n");
	}
	printf("\nRodada: %d\nPontuacao: %d\nVida: %d", *rodada,jogo->pontuacao, jogo->heroi->vida);
}
/***
	*
	*	Esta função irá criar um novo jogo, ou seja, novos mapas e personagens
	*
***/
void criarJogo(Jogo *atual){
}
/***-----------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------
-----------------------
-----------------------                 MAPAS
-----------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
/***--------------------------------------------------------------***/
void telaSelecionarMapa(Jogo *jogo){
	listarMapas();
	char nomeDoMapa[50];
	printf("\nDigite o nome do mapa que deseja selecionar:\n");
	scanf(" %50s", nomeDoMapa);
	while(buscarNomeEmArquivo(nomeDoMapa, "mapas.txt") == 0){
		printf("\nMapa informado nao esta na lista! Digite o nome do mapa que deseja selecionar:\n");
		scanf(" %50s", nomeDoMapa);
	}
	carregarMapaParaOJogo(jogo, nomeDoMapa);
}
void telaCriarMapa(){
	system(CLS);
	char nomeDoMapa[50];
	printf("\nDigite o nome do mapa(ate 50 caracteres e sem espacos): ");
	scanf(" %50s", nomeDoMapa);
	while(buscarNomeEmArquivo(nomeDoMapa, "mapas.txt") > 0){
		printf("\nJa existe um mapa com este nome, por favor, digite outro: \n");
		scanf(" %50s", nomeDoMapa);
	}
	criarMapa(nomeDoMapa);
	salvarMapaNoIndex(nomeDoMapa);
}


/***
	*
	*	Esta função cria um novo mapa segundo o jogador
	*
***/
void criarMapa(char* nomeDoMapa){
	char** matriz;
	int dim = 0, i = 0, j = 0,num;
	char aux[60];
	FILE* arq;
	strcpy(aux,"mapas/");
	strcat(aux,nomeDoMapa);
	strcat(aux,".txt");
	printf("Digite n na forma que (((n*4)+5) vai ser o tamanho real do mapa):");// pedindo o n
	scanf("%d", &num);
	dim = (4*num)+5;
	printf("Digite o mapa  %d x %d:\n",dim,dim);// pedindo o mapa
	matriz = (char**) malloc(dim*sizeof(char*));
	if (matriz == NULL) {
		exit (1);
	}
	for (i = 0; i <= dim; i++) {
		matriz[i] = (char*) malloc((dim+1)*sizeof(char));
		if (matriz[i] == NULL) {
			printf("Erro mapa muito grande\n");// conferindo se o mapa n foi muito grande e a memória n pode ser alocada
			exit (1);
		}
	}
	for (i = 0; i < dim; i++)// pedindo o mapa
		for (j = 0; j < dim+1; j++)// pedindo o mapa
			scanf("%c", &matriz[i][j]);// pedindo o mapa
	arq = fopen(aux,"w");
	for (i = 0; i < dim; ++i)// transformando o limite do mapa em uma cerca
	{
		for (j = 0; j <= dim; ++j)// transformando o limite do mapa em uma cerca
		{
			if(i!=0 && i!= dim-1 && (j==0 || j==dim))matriz[i][j]='|';
			if(i==0 && j==0)matriz[i][j]='/';// transformando o limite do mapa em uma cerca
			else if(i==0 && j==dim)matriz[i][j]='\\';
			else if(i==0)matriz[i][j]='-';// transformando o limite do mapa em uma cerca
			if(i==dim-1 && j==0)matriz[i][j]='\\';
			else if(i==dim-1 && j==dim)matriz[i][j]='/';
			else if (i==dim-1)matriz[i][j]='-';// transformando o limite do mapa em uma cerca
		}
	}
	fprintf(arq, "%d\n", dim);// passando a dimensão do mapa para o arquivo
	for (i = 0; i < dim; i++) {
		for (j = 0; j <= dim; j++){
			fprintf(arq, "%c", matriz[i][j]);// passando mapa para o arquivo
		}
		fprintf(arq,"\n");
	}
	fclose(arq);
}
/***
	*
	* 	Essa função salva o nome do mapa no arquivo  mapas.txt nosso indice de mapas
	*
***/
void salvarMapaNoIndex(char *nomeDoMapa){
	FILE *file = fopen("mapas.txt", "a");
	if(file == NULL){
		printf("\n\tArquivo de mapas nao encontrado!\n");
		exit(1);
	}else{
		system(CLS);
		fprintf((FILE*) file,"\n");
		fputs(nomeDoMapa, (FILE*) file);
	}
	fclose(file);
}
/***
	*
	* 	Essa função carrega os mapas salvos. Apartir do arquivo mapas.txt
	*
***/
void listarMapas(){
	int i,mapaSelecionado;
	char texto[50];
	FILE *file = fopen("mapas.txt", "r");
	if(file == NULL){
		printf("\n\tArquivo de mapas nao encontrado!\n");
		exit(1);
	}else{
		system(CLS);
		printf("\n\nMapas encontrados na base de dados:\n\n");
		while(!feof(file)){
			fgets(texto, 50, (FILE*) file);
			printf("%s", texto);
		}
	}
	fclose(file);
}
/***
	*
	* 	Essa função carrega o mapa do arquivo de texto.
	*
***/
void carregarMapaParaOJogo(Jogo *jogo, char *nomeArquivoMapa){
	int i;
	char texto[300], aux[60];
	strcpy(aux, "mapas/");
	strcat(aux, nomeArquivoMapa);
	strcat(aux, ".txt");
	FILE *file = fopen(aux, "r");
	if(file == NULL){
		printf("\n\tArquivo de mapa %s nao encontrado!\n", nomeArquivoMapa);
		exit(1);
	}else{
		jogo->nomeMapa = (char*) malloc(strlen(nomeArquivoMapa) * sizeof(char));
		strcpy(jogo->nomeMapa, nomeArquivoMapa);
		fscanf(file, "%d", &jogo->tamanho);
		jogo->mapa = (char**) malloc(sizeof(char*) * jogo->tamanho);
		jogo->mapa[0] = (char*) malloc(sizeof(char) * 300);//quebra de linha após o inteiro
		fgets(jogo->mapa[0], 300, (FILE*) file);
		for(i=0; i<jogo->tamanho; i++){
			jogo->mapa[i] = (char*) malloc(sizeof(char) * 300);
			fgets(jogo->mapa[i], 300, (FILE*) file);
		}
	}
}
/***-----------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------
-----------------------
-----------------------                 PERSONAGENS
-----------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------------------------------------------------***/
void telaSelecionarPacoteDePersonagens(Jogo *jogo){
		system(CLS);
		listarPersonagens();
		char nomeDoPacoteDePersonagens[50];
		printf("\n\nDigite o nome do personagem que deseja selecionar:\n");
		scanf(" %50s", nomeDoPacoteDePersonagens);
		while(buscarNomeEmArquivo(nomeDoPacoteDePersonagens, "personagens.txt") == 0){
			printf("\nNome informado nao esta na lista!\nDigite o nome do personagem que deseja selecionar:\n");
			scanf(" %50s", nomeDoPacoteDePersonagens);
		}
		carregarPersonagensParaOJogo(nomeDoPacoteDePersonagens, jogo);
}
void telaCriarPacoteDePersonagens(Jogo *jogo){
		int i;
		char nomePacotePersonagens[50];
		system(CLS);
		printf("Bem vindo a criacao de um novo pacote de personagens!!\n");
		printf("Digite o nome do pacote de personagens(ate 50 caracteres, sem espacos): \n");
		scanf(" %50s", nomePacotePersonagens);
		while(buscarNomeEmArquivo(nomePacotePersonagens, "personagens.txt") > 0){
			printf("Ja existe um pacote de personagens com este nome, por favor, digite outro: \n");
			scanf(" %50s", nomePacotePersonagens);
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
			printf("Digite a quantidade de vida que o monstro possui: \n");
			scanf("%d", &jogo->monstros[i].vida);
			printf("Digite a quantidade de ataque que o monstro possui: \n");
			scanf("%d", &jogo->monstros[i].ataque);
			printf("Digite a quantidade de defesa que o monstro possui: \n");
			scanf("%d", &jogo->monstros[i].defesa);
		}
		salvarPersonagemNoIndex(nomePacotePersonagens);
		criarPacotePersonagens(nomePacotePersonagens, jogo->heroi, jogo->monstros);
}
/***
	*
	* 	Essa função salva o nome do pacote de personagens em personagens.txt nosso indice de personagens.
	*
***/
void salvarPersonagemNoIndex(char *nomeDoPersonagem){
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
		char aux[67];
		int i;
		strcpy(aux, "personagens/");
		strcat(aux, nomePacotePersonagens);
		strcat(aux, ".bin");
		FILE *file = fopen(aux, "wb");
		if(file == NULL){
				printf("\n\tNão foi possível criar um pacote de personagens!\n");
				exit(1);
		}else{
				fwrite(heroi, sizeof(Personagem), 1, file);//"Cria"/"Limpa"
				fclose(file);
				file = fopen(aux, "ab");//Abre em modo append
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
void listarPersonagens(){
	int i,personagemSelecionado;
	char texto[50];
	FILE *file = fopen("personagens.txt", "r");
	if(file == NULL){
		printf("\n\tArquivo de personagens nao encontrado!\n");
		exit(1);
	}else{
		system(CLS);
		printf("\n\nPacotes de personagens encontrados na base de dados:\n\n");
		while(!feof(file)){
			fgets(texto, 50, (FILE*) file);
			printf("%s", texto);
		}
	}
}
/***
	*
	* 	Essa função carrega os personagens do arquivo binário.
	*
***/
void carregarPersonagensParaOJogo(char *nomeDoArquivo, Jogo *jogo){
	char aux[67];
	int i;
	strcpy(aux,"personagens/");
	strcat(aux,nomeDoArquivo);
	strcat(aux,".bin");
	FILE *file = fopen(aux, "rb");
	if(file == NULL){
		printf("\n\tArquivo %s nao encontrado!\n", aux);
		exit(1);
	}else{
			jogo->nomePacotePersonagens = (char*) malloc(strlen(nomeDoArquivo) * sizeof(char));
			strcpy(jogo->nomePacotePersonagens, nomeDoArquivo);
			fread(jogo->heroi, sizeof(Personagem), 1, file);
			for(i=0; i<4; i++){
				fread(&jogo->monstros[i], sizeof(Personagem), 1, file);
			}
	}
	fclose(file);
}
/***-----------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------
-----------------------
-----------------------                 ÚTEIS
-----------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------------------------------------------------***/
/***
	*
	* 	Essa função pesquisa por um nome de até 50 caracteres em algum arquivo que armazene strings de até 50 caracteres, linha por linha..
	*
***/
int buscarNomeEmArquivo(char *busca, char *nomeArquivo){
	FILE *file = fopen(nomeArquivo, "r");
	if(file == NULL){
			printf("\n\nTentando buscar a palavra %s no arquivo %s mas o arquivo não foi encontrado!\n", busca, nomeArquivo);
			exit(1);
	}else{
			char texto[50];
			while(!feof(file)){
					//fgets(texto,50, (FILE*) file); isso daqui pega o \n tbm..
					fscanf(file, " %[^\n]", texto);
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
void exibirArquivo(char *nomeDoArquivo){// função que mostra um arquivo na tela
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
	*	Esta função retorna o nome do mapa que o jogador escolheu
	*
	*
***/
void nomeEmArquivoPorLinha(int numeroDaLinha, char *nomeDoArquivo){
	int i;
	char* nome = (char*) malloc(100*sizeof(char));
	FILE* arq;
	arq = fopen(nomeDoArquivo,"r");
 	for (i = 0; i <= numeroDaLinha; ++i){
 		fgets(nome,100,arq);
 	}
 	fclose(arq);
 	for (i = 0; nome[i] !='\n'; ++i);
 	nome[i]='\0';
}
