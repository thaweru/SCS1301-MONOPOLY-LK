#include "types.h"
#include <stdio.h>

int main(int argc, char *argv[]){
	puts("MONOPOLY-LK Simulation");
    game g;
	//plyr player[PLAYERS];
	g = init_game(argc, argv);
	puts("");
	start_game(g.player, PLAYERS, START_CASH);
	puts("");
	run_game(MAX_ROUNDS, PLAYERS, g);
    puts("");
    winner_of_game(g);
//	for (int i=0; i < 24; i++) printf("%i, duble = %i\n", dice(&duble), duble);
}
