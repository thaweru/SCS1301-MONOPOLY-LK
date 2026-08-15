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
	if (s->type == PROPERTY || s->type == RAILWAY || s->type == UTILITY){
		switch (p->strat){
			case AGGRESSIVE_INVESTOR:
				if ((p->cash-MAX_RENT) > s->buyPrice) return 1; break;
			case CONSERVATIVE_BANKER:
				if (p->cash/2 >= s->buyPrice) return 1; break;
			case RISK_TAKER:
				if (p->cash > s->buyPrice) return 1; break;
			case OPPORTUNISTIC_TRADER:
				if (projected_appriciation(s) > (s->houseCost*4 + s->hotelCost)) return 1; break;
		}
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

instyp get_insurance_policy(plyr *p, sqr *s){
	instyp getting;
	switch (p->strat){
		case AGGRESSIVE_INVESTOR:
			if (s->houses > 4){
				getting = COMPREHENSIVE;
			}else{
				getting = BASIC;
			}
			break;
		case CONSERVATIVE_BANKER:
			getting = COMPREHENSIVE;
			break;
		case RISK_TAKER:
			return NO_INS;
			break;
		case OPPORTUNISTIC_TRADER:
			if (s->group == YELLOW || s->group == GREEN || s->group == DARK_BLUE){
				getting = COMPREHENSIVE;
			}
			break;
	}
	int premium = 0;
	switch (getting){
		case NO_INS: return NO_INS; break;
		case BASIC:
			premium = (s->buyPrice * BASIC_INS_PRM)/100;
			break;
		case COMPREHENSIVE:
			premium = (s->buyPrice * COMP_INS_PRM)/100;
			break;
		case BIZ_INTRPT:
			premium = (s->buyPrice * BIZ_INTRPT_PRM)/100;
			break;
	}
	if (premium < p->cash){
		p->cash -= premium;
		switch (s->insured){
			case BASIC:
				printf("Basic Insurence Purchased\n"); break;
			case COMPREHENSIVE:
				printf("Comprehensive Insurance Purchased\n"); break;
			case BIZ_INTRPT:
				printf("Business Interruption Insurance\n");
			default: break;
		}
		printf("Property : %s\nPremium : LKR %d\n", s->name, premium);
		return getting;
	}else{
		return NO_INS;
	}
}