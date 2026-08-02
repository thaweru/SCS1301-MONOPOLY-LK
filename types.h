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

typedef enum sqrType{
	START,
	PROPERTY,
	RAILWAY,
	UTILITY,
	EVENT,
	SPECIAL,
	BANK
} sqrTyp;

typedef struct Square{
	sqrTyp type;
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
bd init_game(int n, char **arr);
void start_game();
#endif
