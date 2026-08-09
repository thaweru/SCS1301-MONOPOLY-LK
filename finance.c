#include "types.h"

int net_worth(plyr *p, bd *bord){
    int worth = 0;
    for (int i =0; i < 40; i++){
        if (bord->s[i].owner == p){
            worth += bord->s[i].buyPrice;
        }
    }
    worth += p->cash;
    return worth;
}
