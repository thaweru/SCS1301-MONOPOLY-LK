#ifndef TYPES_H
#define TYPES_H

//Player attributes
typedef struct Player{
	char name[24];
	char strategy;
	int cash;
	int position;
	char InJail;
	int jailTurns;
//	struct Loan;
} plyr;

typedef union SquareType{
//	struct Property prpty;
//	struct 
} sqrtype;

typedef struct Square{
	char type;
	char name[42];
	char group;
	int buyPrice;
	int baseRent;
	int houseCost;
	int hotelCost;
	plyr *owner;
	char houses;
} sqr;

typedef struct Board{
	sqr s[40];
} bd;

int dice(char *duble);
void init_game(int n, char **arr);
#endif
