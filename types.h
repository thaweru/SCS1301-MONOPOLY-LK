#ifndef TYPES_H
#define TYPES_H


//Main structure of the program
struct Game{
	struct Player player[4];
	struct Board board;
	int currentRound = 0;
	struct EconomicState economy;
};

void startGame();
void runRound();
char isGameOver();
struct Player determineWinner();

//Player attributes
struct Player{
	char *name;
	int cash;
	int position;
	char InJail;
	int jailTurns;
	struct Property property[];
	struct Loan loan;
	struct InsurancePolicy[] policies;
	struct PlayerStratergy stratergy;
};

#endif
