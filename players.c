#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

plyr spawn_player(char n){
    plyr p;
	switch (n%4){
		case 0:
		    strcpy(p.name, "Aggressive Investor");
            p.strat = AGGRESSIVE_INVESTOR;
		    break;
		case 1:
		    strcpy(p.name, "Conservative Banker");
            p.strat = CONSERVATIVE_BANKER;
		    break;
		case 2:
		    strcpy(p.name, "Risk Taker");
            p.strat = RISK_TAKER;
		    break;
		case 3:
		    strcpy(p.name, "Opportunistic Trader");
            p.strat =  OPPORTUNISTIC_TRADER;
		    break;
	}
    p.cash = START_CASH; p.income = 0; p.pos = 0; p.InJail = 0;
    p.railutil = 0; p.properties = 0; p.lastBuy = NULL; p.NW = 0;
	return p; 
}

int canBuy(plyr *p, sqr *s){
	if (s->type != PROPERTY || s->type != RAILWAY || s->type != UTILITY) return 0;
	switch (p->strat){
		case AGGRESSIVE_INVESTOR:
			if ((p->cash-MAX_RENT) > s->buyPrice){
                if (s->type == PROPERTY){
                    if (plyrhasGroup(p, s->group) || p->properties == 0) return 1;
                }else{
                    return 1;
                }
            }break;
		case CONSERVATIVE_BANKER:
			if (p->cash/2 >= s->buyPrice) return 1; break;
		case RISK_TAKER:
			if (p->cash > s->buyPrice) return 1; break;
		case OPPORTUNISTIC_TRADER:
			if (projected_appriciation(s) > (s->houseCost*4 + s->hotelCost)) return 1; break;
	}
	return 0;
}

int auction_bid(plyr p, sqr s, int nextBid){
	if (p.cash < nextBid) return 0;
	char decision = 0;
	switch (p.strat){
		case AGGRESSIVE_INVESTOR:
			if (nextBid < (s.buyPrice * 120)/100) decision = 1;
			break;
		case CONSERVATIVE_BANKER:
			if (nextBid < s.buyPrice) decision = 1;
			break;
		case RISK_TAKER:
			if (nextBid < p.cash) decision = 1;
			break;
		case OPPORTUNISTIC_TRADER:
			if (nextBid <= s.buyPrice) decision = 1;
			break;
		default: break;
	}
	if (decision == 1){
		printf("%s bids LKR %d\n", p.name, nextBid);
		return nextBid;
	}//else{
		//printf("%s passes.\n", p.name);
	//}
	return 0;
}

char plyrhasGroup(plyr *p, sqrgrp group){
    char has = 0;
    sqr *ptr = p->lastBuy;
    while(ptr != NULL){
        if (ptr->group == group) has = 1;
        ptr = ptr->prevBuy;
    }
    return has;
}
