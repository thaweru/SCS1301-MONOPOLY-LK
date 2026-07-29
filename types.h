#ifndef TYPES_H
#define TYPES_H

struct Player;
struct Property;
struct InsurancePolicy;
struct Loan;
struct PlayerStratergy;

//Player attributes
struct Player{
	int cash;
	int position;
	char InJail;
	int jailTurns;
//	struct Property property[];
//	struct Loan loan;
//	struct InsurancePolicy policies[];
//	struct PlayerStratergy stratergy;
	char name[];
};

//Property attributes
struct Property{
	int purchasePrice;
	int morgageValue;
	int baseRental;
	int houseCost;
	int hotelCost;
//	enum PropertyGroup group;
	struct Player *owner;
	char morgaged;
	int numHouses;
	char hasHotel;
	int condition;
	int age;
};
//Main structure of the program
struct Game{
	struct Player player[4];
//	struct Board board;
	int currentRound ;//= 0;
//	struct EconomicState economy;
};

void startGame();
void runRound();
char isGameOver();
struct Player determineWinner();


void takeTurn();
void purchaseProperty(struct Property);
void payRent(int);
void declareBankrupt();
int calculateNetWorth();

//Board attributes
struct Board{
//	struct Square square[40];
};

void initializeBoard();
struct Square getSquare(int);

struct Square{
	int index;
	char name[];
//	union SquareType type;
};


int calculateRent(int);
void construct();
void morgage();
void renovate();
int dice(char *duble);
#endif
