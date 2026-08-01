#include "types.h"
#include <stdio.h>

int main(int argc, char *argv[]){
	plyr player[4];
	bd board = init_game(argc, argv);
	int currRound;
	start_game();
	char duble;
//	for (int i=0; i < 24; i++) printf("%i, duble = %i\n", dice(&duble), duble);
}
