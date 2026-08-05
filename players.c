
#include "types.h"
#include <stdlib.h>

plyr spawn_player(char n){
	plyr p;
	switch (n){
		case 1:
		strcpy(p.name, "Aggressive Investor");
		break;
		case 2:
		strcpy(p.name, "Conservative Banker");
		break;
		case 3:
		strcpy(p.name, "Risk Taker");
		break;
		case 4:
		strcpy(p.name, "Opportunistic Trader");
		break;
	}
	return p;
}
