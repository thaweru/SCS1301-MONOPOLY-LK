#include "types.h"
#include "stdio.h"

void run_game(plyr *P, int N, int n, bd bord){
	int currRound = 0;
	char duble;
	short int rolled, prev;
	while ((currRound < N)){
		for (int i=0; i < n; i++){
			rolled = dice(&duble);
			printf("%s rolled %i\n", P[i].name, rolled);
			prev = P[i].pos;
			P[i].pos += rolled;
			printf("%s moves from Square %i to Square %i\n\n",
				P[i].name, prev, P[i].pos%40
				);
			if (P[i].pos < 40){
				P[i].cash += 2000;
				printf("%s passed GO.\nCollected LKR 2,000.\nCurrent Balance : LKR %d.\n",
					P[i].name, P[i].cash
				);
			P[i].pos = P[i].pos % 40;
			}
		}
		currRound++;
	}
}
