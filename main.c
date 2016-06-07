/***
	*
	*	--- RPG IP 2016 ---
	* @descricao main.c é o arquivo principal do jogo, onde toda a mágica acontece!
	* @desde 06/06/2016	
	* @autores
	*   [
	*	 	Bruno Melo
	* 		Filipe Cumaru
	*		Rafael Santana
	*		Victor Aurélio			
	* 	]
	*
	*
***/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>

typedef struct{	
	char representacao;// A letra que vai representar o personagem no mapa
	char nome[20]; //O Nome do personagem 
	int ataque, defesa, vida;// Variáveis que representam quanto de ataque, defesa ou vida o personagem possui
	int x, y;// Coordenadas X e Y do personagem no mapa
}Personagem;

typedef struct{
	int tamanho; // Tamanho de  Linhas x Colunas (Nossa matriz será sempre quadrada)
	char **mapa; // A matriz que representa o mapa
}Jogo;

/***
	*
	*	Declarando cabeçalhos das funções
	*
***/
char pegarOpcaoMenu();
void exibirMenu(char *ultimo, char *opcao);
void inicarJogo(Personagem *heroi, Personagem *monstros, Jogo *atual);
void carregarJogo(Personagem *heroi, Personagem *monstros, Jogo *atual);
void criarJogo(Personagem *heroi, Personagem *monstros, Jogo *atual);
void substituir(char *texto, char *busca, char c);

int main(int argc, char const *argv[]){	
	Personagem *heroi, *monstros;
	Jogo *jogoAtual;
	
	switch(pegarOpcaoMenu()){// De acordo com a opção que o menu retornar
		case 'w': //Caso seja selecionada a primeira opção do menu
			criarJogo(heroi, monstros, jogoAtual);
			break;
		case 's': //Caso seja selecionada a opção de baixo do menu
			iniciarJogo(heroi, monstros, jogoAtual);
			//iniciarJogo();
			break;
	}

	return 0;
}
/***
	*
	*	Esta retorna qual a opção escolhida pelo usuário no menu.
	*
***/
char pegarOpcaoMenu(){
	char ultimo, opcao;
	ultimo = 'w';
	do{
		system("cls");
		exibirMenu(&ultimo, &opcao);		
	}while((opcao = getch()) && opcao != 13);
	return ultimo;
}
/***
	*
	*	Esta função exibe o menu e de acordo com a atual posição do cursor
	*   ela coloca '>' em cima ou em baixo do texto.
	*
***/
void exibirMenu(char *ultimo, char *opcao){
	FILE *file;
	char texto[20000];
	file = fopen("tela_inicial", "r");
	if(file == NULL){
		#if _WIN32 //Se estiver em um sistema windows
			printf("\a\a");/** Dois beep's para não encontrado, só funciona no windows. **/
		#endif
		printf("\n\tArquivo tela_inicial nao encontrado!\n");
		exit(1);	
	}else{	
		while(!feof(file)){			
			fgets(texto, 20000, (FILE*) file);				
			if(*opcao == 'w' || (*opcao != 's' && *ultimo == 'w')){
				*ultimo = 'w';
				alterarCaracterPrePalavra(texto, "Play", '>');
				alterarCaracterPrePalavra(texto, "Create", ' ');
			}else if(*opcao == 's' || (*opcao != 'w' && *ultimo == 'w')){
				*ultimo = 's';
				alterarCaracterPrePalavra(texto, "Play", ' ');
				alterarCaracterPrePalavra(texto, "Create", '>');
			}
			printf("%s\n", texto);
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
	aux = strstr(texto,busca);		
	if(aux != NULL){
		ptr = aux-2;
		*ptr = c;
	}
}
/***
	*
	*	Esta função irá iniciar um jogo novo.	
	*
***/
void iniciarJogo(){
}
/***
	*
	*	Esta função irá criar um novo jogo, ou seja, novos mapas e personagens
	*
***/
void criarJogo(){
}
/***
	*
	* 	Essa função carrega o mapa do arquivo de texto.
	*
***/
void carregarMapaSalvoEmArquivo(Jogo *mapa){

}
/***
	*
	* 	Essa função carrega os personagens do arquivo binário.
	*
***/
void carregarPersonagensSalvosEmArquivo(Personagem *heroi, Personagem *monstros){

}