#include "types.h"
#include <stdio.h>

void add_property_to_player(plyr *p, sqr *s){
    s->owner = p;
    s->prevBuy = p->lastBuy;
    p->lastBuy = s;
    switch (s->type){
        case PROPERTY: p->properties++; break;
        case RAILWAY: p->railutil++; break;
        case UTILITY: p->railutil += 16; break;
        default: break;
    }
}

int total_assets(plyr *p){
    if (p->cash < 0) return -1;
    int worth = p->cash;
    sqr *ptr = p->lastBuy;
    while(ptr != NULL){
        worth += ptr->buyPrice;
        if(ptr->type == PROPERTY){
            if (ptr->houses > 0 && ptr->houses <= 4){
                worth += ptr->houses * ptr->houseCost;
            } else if (ptr->houses == 5){
                worth += (4 * ptr->houseCost) + ptr->hotelCost;
            }
        }
        ptr = ptr->prevBuy;
    }
    return worth;
}

int net_worth(plyr *p){
    return total_assets(p);
}

int isBankrupt(plyr *p){
    if (p->cash < 0) return 1;
    return 0;
}

void auction(sqr *s, game *g){
    int currentBid = s->buyPrice / 2;
    plyr *highestBidder = NULL;
    int bidsInRound;
    printf("Auction for %s starting at LKR %d\n", s->name, currentBid);
    do {
        bidsInRound = 0;
        for (int i = 0; i < PLAYERS; i++){
            if (g->player[i].cash < 0) continue;
            if (&g->player[i] == highestBidder) continue;
            int nextBid = currentBid + BID_INCREMENT;
            if (auction_bid(g->player[i], *s, nextBid)){
                highestBidder = &g->player[i];
                currentBid = nextBid;
                bidsInRound++;
            }
        }
    } while (bidsInRound > 0);

    if (highestBidder != NULL){
        printf("%s wins the auction for %s at LKR %d\n", highestBidder->name, s->name, currentBid);
        highestBidder->cash -= currentBid;
        add_property_to_player(highestBidder, s);
    }else{
        printf("No bids were placed for %s\n", s->name);
    }
}

int projected_appreciation(sqr *s){
    int appreciation = 0;
    switch (s->group){
        case BROWN:
            appreciation = 1000;
            break;
        case LIGHT_BLUE:
            appreciation = 2000;
            break;
        case PINK:
            appreciation = 3000;
            break;
        case ORANGE:
            appreciation = 4000;
            break;
        case RED:
            appreciation = 5000;
            break;
        case YELLOW:
            appreciation = 6000;
            break;
        case GREEN:
            appreciation = 7000;
            break;
        case DARK_BLUE:
            appreciation = 8000;
            break;
        default:
            appreciation = 0;
            break;
    }
    return appreciation;
}
