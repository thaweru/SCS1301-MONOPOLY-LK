#include "types.h"
#include <stdio.h>

int net_worth(plyr *p, game *g){
    int worth = 0;
    for (int i =0; i < 40; i++){
        if (g->square[i].owner == p){
            worth += g->square[i].buyPrice;
        }
    }
    worth += p->cash;
    return worth;
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
            if (g->player[i].cash > currentBid){
                highestBidder = &g->player[i];
                currentBid = auction_bid(g->player[i], *s, nextBid);
                count++;
            }
        }
    } while (count < 1);
    if (highestBidder != NULL){
        printf("%s wins the auction for %s at LKR %d\n", highestBidder->name, s->name, currentBid);
        highestBidder->cash -= currentBid;
        s->owner = highestBidder;
    }else{
        printf("No bids were placed for %s\n", s->name);
    }
}
