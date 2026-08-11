
#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

plyr spawn_player(char n){
	plyr p;
	switch (n%4){
		case 0:
		p = (plyr){"Aggressive Investor", AGGRESSIVE_INVESTOR, START_CASH, 0, 0, 0, 0};
		break;
		case 1:
		p = (plyr){"Conservative Banker", CONSERVATIVE_BANKER, START_CASH, 0, 0, 0, 0};
		break;
		case 2:
		p = (plyr){"Risk Taker", RISK_TAKER, START_CASH, 0, 0, 0, 0};
		break;
		case 3:
		p = (plyr){"Opportunistic Trader", OPPORTUNISTIC_TRADER, START_CASH, 0, 0, 0, 0};
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

int auction_bid(plyr p, sqr s, int nextBid){
	if (p.cash < nextBid) return -1;
	char decision = 0;
	switch (p.strat){
		case AGGRESSIVE_INVESTOR:
			if (nextBid < (s.buyPrice * 120)/100) decision = 1;
			break;
		case CONSERVATIVE_BANKER:
			if (nextBid < s.buyPrice) decision = 1;
			break;
		case RISK_TAKER:
			decision = 1;
			break;
		case OPPORTUNISTIC_TRADER:
			if (nextBid <= s.buyPrice) decision = 1;
			break;
		default: break;
	}
	if (decision){
		printf("%s bids LKR %d\n", p.name, nextBid);
		return nextBid;
	}else{
		printf("%s withdraws.\n", p.name);
	}
	return -1;
}
