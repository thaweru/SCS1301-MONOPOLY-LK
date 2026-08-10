
#include "types.h"
#include <stdlib.h>

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
        p.strat = OPPORTUNISTIC_TRADER;
		break;
	}
	return p;
}

int canBuy(plyr *p, sqr *s){
	if (p->cash > s->baseRent){
		switch (s->type){
			case PROPERTY:
			case RAILWAY:
			case UTILITY:
				return 1;
				break;
			default: break;
		}
	}
	return 0;
}

int decide_bid(plyr *p, sqr *s, int nextbid){
    //if (nextbid > p->cash) return -1;
    switch (p->strat){
        case AGGRESSIVE_INVESTOR:
            if (nextbid <= (s->buyPrice * 120)/100) return nextbid;
            return -1;
        case RISK_TAKER:
            return nextbid;
        case CONSERVATIVE_BANKER:
            if (nextbid < s->buyPrice) return nextbid;
            return -1;
        case OPPORTUNISTIC_TRADER:
            if (nextbid <= s->buyPrice) return nextbid;
            return -1;
        default:
            return -1;
    }
}
