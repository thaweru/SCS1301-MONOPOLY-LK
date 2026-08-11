
#include "types.h"
#include <stdlib.h>
#include <string.h>

plyr spawn_player(char n){
	plyr p;
	switch (n%4){
		case 0:
		strcpy(p.name, "Aggressive Investor");
		break;
		case 1:
		strcpy(p.name, "Conservative Banker");
		break;
		case 2:
		strcpy(p.name, "Risk Taker");
		break;
		case 3:
		strcpy(p.name, "Opportunistic Trader");
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

int auction_bid(bid highbid, sqr s, plyr *p){
    if ((highbid.bidder != p)&&(highbid.value < s.buyPrice)){
        return highbid.value + BID_INCREMENT;
    }
}
