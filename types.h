#ifndef TYPES_H
#define TYPES_H

//Player attributes
typedef struct Player{
	char name[24];
	char strategy;
	int cash;
	char pos;
	char InJail;
	char railutil;
//	struct Loan;
} plyr;

typedef enum sqrType{
	EMPTY = -1,
	START,
	PROPERTY,
	RAILWAY,
	UTILITY,
	EVENT,
	SPECIAL,
	INSURANCE,
	BANK,
    TAX
} sqrTyp;

typedef enum propertyGroup{
	NONE = -1,
	BROWN,
	LIGHT_BLUE,
	PINK,
	ORANGE,
	RED,
	YELLOW,
	GREEN,
	DARK_BLUE,
	JAIL,
	GO_TO_JAIL,
	EVENT_CARD,
	CD_FUND
}sqrgrp;

typedef struct Square{
	sqrTyp type;
	char name[42];
	sqrgrp group;
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
void start_game(plyr *plyrs, int n, int cash);
plyr spawn_player(char n);
void run_game(plyr *P, int N, int n, bd bord);
void landing_action(plyr *p, sqr *s, int roll);
int canBuy(plyr *p, sqr *s);
#endif
