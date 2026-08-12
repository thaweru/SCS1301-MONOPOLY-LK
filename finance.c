#include "types.h"
#include <stdio.h>

int net_worth(plyr *p, game *g){
    int worth = 0;
    //for (int i =0; i < 40; i++){if (g->square[i].owner == p){worth += g->square[i].buyPrice;}}
    sqr *ptr = p->lastBuy;
    while(ptr != NULL){
        worth += ptr->buyPrice;
        printf("owns %s\n", ptr->name);
        ptr = ptr->prevBuy;
    }
    worth += p->cash; puts("");
    return worth;
}

int isBankrupt(plyr *p){
    if (p->cash <= 0) return 1;
    return 0;
}

void auction(sqr *s, game *g){
    int currentBid = s->buyPrice/2, nextBid;
    char count = 0;
    plyr *highestBidder = NULL;
    printf("Auction for %s starting at LKR %d\n", s->name, currentBid);
    do{
        count = 0;
        nextBid = currentBid + BID_INCREMENT;
        for (int i = 0; i < PLAYERS; i++){
            if ((auction_bid(g->player[i], *s, nextBid))){
                highestBidder = &g->player[i];
                currentBid = nextBid;
                count++;
            }
        }
    } while (count > 1);
    if (highestBidder != NULL){
        printf("%s wins the auction for %s at LKR %d\n", highestBidder->name, s->name, currentBid);
        highestBidder->cash -= currentBid;
        s->owner = highestBidder;
    }else{
        printf("No bids were placed for %s\n", s->name);
    }
}

int projected_appriciation(sqr *s){
    int appriciation = 0;
    switch (s->group){
        case BROWN:
            appriciation = 1000;
            break;
        case LIGHT_BLUE:
            appriciation = 2000;
            break;
        case PINK:
            appriciation = 3000;
            break;
        case ORANGE:
            appriciation = 4000;
            break;
        case RED:
            appriciation = 5000;
            break;
        case YELLOW:
            appriciation = 6000;
            break;
        case GREEN:
            appriciation = 7000;
            break;
        case DARK_BLUE:
            appriciation = 8000;
            break;
        default:
            appriciation = 0;
            break;
    }
    return appriciation;
}