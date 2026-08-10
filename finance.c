#include "types.h"

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

char bankrupt(plyr p){
    if (p.cash <=0) return 1;
    return 0;
}
