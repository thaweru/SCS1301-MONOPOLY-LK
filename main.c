#include "types.h"
#include <stdio.h>

int main(int argc, char *argv[]){
	puts("MONOPOLY-LK Simulation");
	plyr player[4];
	bd board = init_game(argc, argv);
	puts("");
	int currRound = 0;
	start_game(player, 4, 30000);
	char duble;
	while ((currRound < 500)){
		for (int i=0; i < 4; i++){

		}
		currRound++;
	}
//	for (int i=0; i < 24; i++) printf("%i, duble = %i\n", dice(&duble), duble);
}
