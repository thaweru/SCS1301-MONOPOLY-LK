#include "types.h"
#include "stdio.h"
#define PASS_GO_CASH 2000

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
			printf("%s moves from Square %i to Square %i\n",
				P[i].name, prev, (P[i].pos%40)
				);
			landing_action(&P[i], &bord.s[P[i].pos%40]);
			puts("");
		}
		currRound++;
	}
}

void landing_action(plyr *p, sqr *s){
	if (p->pos > 40){
		p->pos = p->pos % 40;
		p->cash += PASS_GO_CASH;
		printf("%s passed GO.\nCollected LKR %i.\nCurrent Balance : LKR %d.\n", 
			p->name, PASS_GO_CASH, p->cash);
			}
	if (s->owner != NULL){
		p->cash -= s->baseRent;
		s->owner->cash += s->baseRent;
		printf("%s landed on %s\nRent paid : LKR %d\nOwner : %s\n",
			p->name, s->name, s->baseRent, s->owner->name);
	}
	if (s->owner == NULL){
		p->cash -= s->buyPrice;
		s->owner = p;
		printf("%s purchased %s for LKR %d.\nCurrent Balance : LKR %d\n", 
			p->name, s->name, s->buyPrice, p->cash);
	}
}
