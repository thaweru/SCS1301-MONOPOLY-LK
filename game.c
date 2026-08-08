#include "types.h"
#include "stdio.h"
#define PASS_GO_CASH 2000

void run_game(plyr *P, int N, int n, bd bord){
	int currRound = 0;
	char duble;
	short int rolled, prev;
	while ((currRound < N)){
		for (int i=0; i < n; i++){
			rolled = 1; //dice(&duble);
			printf("%s rolled %i\n", P[i].name, rolled);
			if (P[i].InJail == 0){
				prev = P[i].pos;
				P[i].pos += rolled;
				printf("%s moves from Square %i to Square %i\n",
				P[i].name, prev, (P[i].pos%40)
				);
				landing_action(&P[i], &bord.s[P[i].pos%40], rolled);
			}else{
				if (duble == 1){
					printf("%s rolled doubles. Released from jail.\n", P[i].name);
					P[i].InJail = 0;
				}else{
                    printf("%s is in Jail\n", P[i].name);
                }
			}
			puts("");
		}
        printf("=============================================\n");
        printf("Round %d Summery\n", currRound+1);
        printf("=============================================\n");
        for (int i=0; i < n; i++){
            printf("%s\nCash : %d\n", P[i].name, P[i].cash);
            printf("---------------------------------------------\n");
        }
		currRound++;
	}
}

void landing_action(plyr *p, sqr *s, int roll){
	//Collect cash from passing GO
	if (p->pos >= 40){
		p->pos = p->pos % 40;
		p->cash += PASS_GO_CASH;
		printf("%s passed GO.\nCollected LKR %i.\nCurrent Balance : LKR %d.\n", 
			p->name, PASS_GO_CASH, p->cash);
		}
	//Pay rent to owner of square
	printf("%s landed on %s\n", p->name, s->name);
	if ((s->owner != NULL)&&(s->owner != p)){
		int rentdue = 0;
		switch (s->type){
			case PROPERTY:
				rentdue = s->baseRent;
				break;
			case RAILWAY:
				switch (s->owner->railutil%16){
					case 1: rentdue = 250; break;
					case 2: rentdue = 500; break;
					case 3: rentdue = 1000; break;
					case 4: rentdue = 2000; break;
				}
				break;
			case UTILITY:
				switch (s->owner->railutil/16){
					case 1: rentdue = 4 * roll; break;
					case 2: rentdue = 10 * roll; break;
				}
			default: break;
		}
		if (rentdue > 0){
			p->cash -= rentdue;
			s->owner->cash += rentdue;
			printf("Rent paid : LKR %d\nOwner : %s\n",
				rentdue, s->owner->name);
		}
	}
		switch (s->group){
			case GO_TO_JAIL:
				printf("%s is immidiately transferred to Jail\n", p->name);
				p->InJail = 1;
				p->pos = 10;
				break;
			case JAIL:
				if ((p->InJail > 3)&&(p->InJail != 0)){
					p->InJail = 0;
					printf("%s is released from Jail\n", p->name);
				}else{
					p->InJail++;
					printf("%s is in Jail\n", p->name);
				}
			default: break;

		}
	//Buy the square when no one owns it
	if ((s->owner == NULL)&&(canBuy(p, s))){
		p->cash -= s->buyPrice;
		s->owner = p;
		switch (s->type){
			case RAILWAY: p->railutil++; break;
			case UTILITY: p->railutil += 16; break;
			default: break;
		}
		printf("%s purchased %s for LKR %d.\nCurrent Balance : LKR %d\n", 
			p->name, s->name, s->buyPrice, p->cash);
	}
}
