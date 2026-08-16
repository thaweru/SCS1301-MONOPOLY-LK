#include "types.h"
#include <stdio.h>

void round_end_plyr_stat(plyr *p);

void run_game(int N, int n, game *g){
    plyr *P = g->player;
	g->currRound = 0;
	char duble;
	short int rolled, prev;

	while (g->currRound < N){
        int active_players = 0;
        for (int i = 0; i < n; i++){
            if (P[i].cash >= 0) active_players++;
        }
        if (active_players <= 1) break;

		for (int i = 0; i < n; i++){
            if (P[i].cash < 0){
                continue; // Skip bankrupt players
            }
			rolled = dice(&duble);
			printf("%s rolled %i\n", P[i].name, rolled);
			if (P[i].InJail == 0){
				prev = P[i].pos;
				P[i].pos += rolled;
				printf("%s moves from Square %i to Square %i\n",
				    P[i].name, prev, (P[i].pos % 40)
				);
				landing_action(&P[i], &g->square[P[i].pos % 40], rolled, g);
			}else{
				if (duble == 1){
					printf("%s rolled doubles. Released from jail.\n", P[i].name);
					P[i].InJail = 0;
                    prev = P[i].pos;
                    P[i].pos += rolled;
                    printf("%s moves from Square %i to Square %i\n",
                        P[i].name, prev, (P[i].pos % 40)
                    );
                    landing_action(&P[i], &g->square[P[i].pos % 40], rolled, g);
				}else{
                    P[i].InJail++;
                    if (P[i].InJail > 3){
                        printf("%s served 3 turns in Jail and is released.\n", P[i].name);
                        P[i].InJail = 0;
                        prev = P[i].pos;
                        P[i].pos += rolled;
                        printf("%s moves from Square %i to Square %i\n",
                            P[i].name, prev, (P[i].pos % 40)
                        );
                        landing_action(&P[i], &g->square[P[i].pos % 40], rolled, g);
                    }else{
                        printf("%s is in Jail\n", P[i].name);
                    }
                }
			}
			puts("");
		}
		g->currRound++;
        printf("=============================================\n");
        printf("Round %d Summary\n", g->currRound);
        printf("=============================================\n");
        for (int i = 0; i < PLAYERS; i++){
            round_end_plyr_stat(&g->player[i]);
        }
	}
}

void round_end_plyr_stat(plyr *p){
    if (p->cash < 0){
        printf("%s\nBankrupt\n", p->name);
        p->NW = -1;
        printf("Net Worth : LKR -1\n");
        printf("Properties : 0\n");
    } else {
        printf("%s\nCash : LKR %d\n", p->name, p->cash);
        p->NW = net_worth(p);
        printf("Net Worth : LKR %i\n", p->NW);
        printf("Properties : %d\n", p->properties);
        if (p->railutil > 0) printf("Railway : %d Utilities : %d\n", p->railutil % 16, p->railutil / 16);
    }
    printf("---------------------------------------------\n");
}

void landing_action(plyr *p, sqr *s, int roll, game *g){
	//Collect cash from passing GO
	if (p->pos >= 40){
		p->pos = p->pos % 40;
		p->cash += PASS_GO_CASH;
        p->income += PASS_GO_CASH;
		printf("%s passed GO.\nCollected LKR %i.\nCurrent Balance : LKR %d.\n", 
			p->name, PASS_GO_CASH, p->cash);
	}
	//Pay rent to owner of square
	printf("%s landed on %s\n", p->name, s->name);
	if ((s->owner != NULL) && (s->owner != p) && (s->owner->cash >= 0)){
		int rentdue = 0;
		switch (s->type){
			case PROPERTY:
                if (s->houses == 0){
				    rentdue = s->baseRent;
                    if (isMonopoly(g, s->owner, s)){
                        rentdue *= 2;
                    }
                } else if (s->houses == 1){
                    rentdue = s->baseRent * 5;
                } else if (s->houses == 2){
                    rentdue = s->baseRent * 15;
                } else if (s->houses == 3){
                    rentdue = s->baseRent * 40;
                } else if (s->houses == 4){
                    rentdue = s->baseRent * 70;
                } else if (s->houses == 5){
                    rentdue = s->baseRent * 100;
                }
				break;
			case RAILWAY:
				switch (s->owner->railutil % 16){
					case 1: rentdue = 250; break;
					case 2: rentdue = 500; break;
					case 3: rentdue = 1000; break;
					case 4: rentdue = 2000; break;
                    default: rentdue = 250; break;
				}
				break;
			case UTILITY:
				switch (s->owner->railutil / 16){
					case 1: rentdue = 4 * roll; break;
					case 2: rentdue = 10 * roll; break;
                    default: rentdue = 4 * roll; break;
				}
				break;
			default: break;
		}
		if (rentdue > 0){
            if (p->cash >= rentdue){
			    p->cash -= rentdue;
			    s->owner->cash += rentdue;
                s->owner->income += rentdue;
			    printf("Rent paid : LKR %d\nOwner : %s\n",
				    rentdue, s->owner->name);
            }else{
                find_cash(p, rentdue, s->owner);
            }
		}
	}

    // Income tax on TAX square
    if (p->cash >= 0 && s->type == TAX){
        int tax_due = (p->income * INC_TAX) / 100;
        if (tax_due > 0){
            if (p->cash >= tax_due){
                p->cash -= tax_due;
                p->income = 0;
                printf("%s paid income tax of LKR %d\n", p->name, tax_due);
            }else{
                find_cash(p, tax_due, NULL);
            }
        }
    }

    if (p->cash >= 0){
		switch (s->group){
			case GO_TO_JAIL:
				printf("%s is immediately transferred to Jail\n", p->name);
				p->InJail = 1;
				p->pos = 10;
				break;
            case CD_FUND:{
                int tax_due = (total_assets(p) * COM_DEV_TAX) / 100;
                if (tax_due > 0){
                    if (p->cash >= tax_due){
                        p->cash -= tax_due;
                        printf("%s paid community development tax of LKR %d\n", p->name, tax_due);
                    }else{
                        find_cash(p, tax_due, NULL);
                    }
                }
                break;
            }
			default: break;
		}
    }

	//Buy the square when no one owns it
	if (p->cash >= 0 && s->owner == NULL && (s->type == PROPERTY || s->type == RAILWAY || s->type == UTILITY)){
		if (canBuy(p, s)){
			p->cash -= s->buyPrice;
            add_property_to_player(p, s);
			printf("%s purchased %s for LKR %d.\nCurrent Balance : LKR %d\n", 
				p->name, s->name, s->buyPrice, p->cash);
		}else{
			printf("%s will not purchase %s. Initiating Auction.\n", p->name, s->name);
			auction(s, g);
		}
	}

    //Build houses and hotels if monopoly
    if (p->cash >= 0 && isMonopoly(g, p, s) && s->type == PROPERTY && canBuild(g, s)){
        if (s->houses < 4){
            if (p->cash >= s->houseCost){
                printf("%s builds house on %s\n", p->name, s->name);
                p->cash -= s->houseCost;
                s->houses++;
            }
        }else if (s->houses == 4){
            if (p->cash >= s->hotelCost){
                printf("%s builds hotel on %s\n", p->name, s->name);
                p->cash -= s->hotelCost;
                s->houses++;
            }
        }
    }
}

void winner_of_game(plyr *p){
    plyr temp;
    for (int i = 0; i < PLAYERS; i++){
        p[i].NW = net_worth(&p[i]);
    }
    for (int i = 0; i < PLAYERS - 1; i++){
        for (int j = 0; j < PLAYERS - 1 - i; j++){
            if (p[j].NW < p[j + 1].NW){
                temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < PLAYERS; i++){
        printf("%d – %-24s | Net Worth: LKR %d\n", i + 1, p[i].name, p[i].NW);
    }
    printf("\n######## %s wins the game. ########\n\n", p[0].name);
}

void find_cash(plyr *p, int due, plyr *creditor){
    printf("Not enough cash to pay LKR %d\n", due);
    if (p->cash < due){
        if (creditor != NULL && p->cash > 0){
            creditor->cash += p->cash;
            creditor->income += p->cash;
        }
        printf("%s is bankrupt.\n", p->name);
        dissolving_player(p);
        p->cash = -1;
        p->NW = -1;
    }
}

void dissolving_player(plyr *p){
    sqr *ptr = p->lastBuy;
    while(ptr != NULL){
        printf("relinquished %s\n", ptr->name);
        ptr->owner = NULL;
        ptr->houses = 0;
        sqr *tmp = ptr;
        ptr = ptr->prevBuy;
        tmp->prevBuy = NULL;
    }
    p->lastBuy = NULL;
    p->properties = 0;
    p->railutil = 0;
    p->income = 0;
}
