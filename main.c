#include "types.h"
#include <stdio.h>

int main(int argc, char *argv[]){
	puts("MONOPOLY-LK Simulation");
	plyr player[PLAYERS];
	bd board = init_game(argc, argv);
	puts("");
	start_game(player, PLAYERS, 30000);
	puts("");
	run_game(player , MAX_ROUNDS, PLAYERS, board);
//	for (int i=0; i < 24; i++) printf("%i, duble = %i\n", dice(&duble), duble);
}
