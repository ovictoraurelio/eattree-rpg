/*
	[INSTRUÇÕES]:
		- use as teclas w, a, s, d para navegar no menu inicial e no mapa
		- use r para selecionar a opção desejada no menu inicial
		- use e durante a batalha para atacar e r para fugir
*/

//----------------------------------------------------------------

/*

use essa funcao para limpar a
tela nos sistemas operacionais 
mais comuns 

-windows
-linux

*/

void clear_screen(){

	#if _WIN32
		system("cls");
	#else
		system("clear");
	#endif
}
