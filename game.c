#include "types.h"
#include <stdlib.h>
#include <string.h>

void initGame(Game *game, unsigned int seed) {
    srand(seed);
    initBoard(game->board);
    initPlayers(game->players);
    initEvents(game);
    game->currentRound = 1;
    game->totalRoundsRun = 0;
    game->isGameOver = 0;
    game->winnerId = -1;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        game->turnOrder[i] = i;
    }
}

int dice_roll(char *duble){
    int d1 = (rand() % 6) + 1;
    int d2 = (rand() % 6) + 1;
    if (d1 == d2){
        *duble = 1;
    }else{
        *duble = 0;
    }
    return d1 + d2;
}

void determineTurnOrder(Game *game) {
	/**struct keyval{
		Player a;
		int k;
        char same;
	} p[NUM_PLAYERS], temp;
    char duble;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        p[i].a = game->players[i];
        p[i].k = dice_roll(&duble);
        p[i].same = 0;
        printf("%s rolls %d.\n", p[i].a.name, p[i].k);
    }
    int start = 0, stop = NUM_PLAYERS, start_temp, stop_temp;
    resort:
    for (int i=start; i < stop; i++){
        for (int j=start; j < stop-1; j++){
            if (p[j].k < p[j+1].k){
                temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
            if (p[j].k == p[j+1].k){
                p[j] = p[j+1] = 255;
            }
        }
    }
    stop_temp = start; start_temp = stop;
    for (int i=start; i < stop; i++){
        if (p[i].same){
            p[i].k = dice_roll(&duble);
            p[i].same = 0;
            printf("%s re-rolls %d.\n", p[i].a.name, p[i].k);
            if (i < stop_temp) stop_temp = i;
            if (i > start_temp) start_temp = i;
        }
    }
    if ()**/
    int rolls[NUM_PLAYERS];
    int highestRoll = -1;
    int startingPlayer = 0;

    /* Roll 2 dice per player */
    int tie = 1;
    while (tie) {
        tie = 0;
        highestRoll = -1;
        for (int i = 0; i < NUM_PLAYERS; i++) {
            int d1 = (rand() % 6) + 1;
            int d2 = (rand() % 6) + 1;
            rolls[i] = d1 + d2;
            printf("%s rolls %d.\n", game->players[i].name, rolls[i]);
            if (rolls[i] > highestRoll) {
                highestRoll = rolls[i];
                startingPlayer = i;
            }
        }

        /* Check ties for highest */
        int highCount = 0;
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (rolls[i] == highestRoll) highCount++;
        }
        if (highCount > 1) {
            tie = 1;
            printf("Tie for highest roll. Rerolling...\n\n");
        }
    }

    printf("\n%s will begin the game.\n\n", game->players[startingPlayer].name);
    printf("Turn order:\n");
    for (int i = 0; i < NUM_PLAYERS; i++) {
        int pid = (startingPlayer + i) % NUM_PLAYERS;
        game->turnOrder[i] = pid;
        printf("%s\n", game->players[pid].name);
    }
    printf("\n");
}

void triggerAuction(Game *game, int propertyIndex) {
    Square *sq = &game->board[propertyIndex];
    int marketVal = getPropertyMarketValue(game, propertyIndex);
    int openingBid = marketVal / 2;
    if (openingBid < 250) openingBid = 250;

    Auction auction;
    auction.propertyIndex = propertyIndex;
    auction.openingBid = openingBid;
    auction.currentBid = 0;
    auction.highestBidderId = -1;

    for (int i = 0; i < NUM_PLAYERS; i++) {
        auction.activeParticipants[i] = game->players[i].bankrupt ? 0 : 1;
    }

    printf("\nAuction Started.\n");
    printf("Property : %s\n", sq->name);
    printf("Opening Bid : LKR %d.\n\n", openingBid);

    int activeCount = 0;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (auction.activeParticipants[i]) activeCount++;
    }

    int turnIdx = 0;
    while (activeCount > 1 || (activeCount == 1 && auction.currentBid == 0)) {
        int pid = game->turnOrder[turnIdx % NUM_PLAYERS];
        turnIdx++;

        if (!auction.activeParticipants[pid]) continue;

        int bid = playerDecideAuctionBid(game, pid, &auction);
        int minRequired = (auction.currentBid == 0) ? auction.openingBid : (auction.currentBid + 250);

        if (bid >= minRequired && bid <= game->players[pid].cash) {
            auction.currentBid = bid;
            auction.highestBidderId = pid;
            printf("%s bids LKR %d.\n", game->players[pid].name, bid);
        } else {
            auction.activeParticipants[pid] = 0;
            printf("%s withdraws.\n", game->players[pid].name);
            activeCount--;
        }
    }

    if (auction.highestBidderId >= 0 && auction.currentBid > 0) {
        Player *winner = &game->players[auction.highestBidderId];
        winner->cash -= auction.currentBid;
        sq->ownerId = auction.highestBidderId;
        sq->age = 0;
        printf("%s wins the auction.\n", winner->name);
    } else {
        printf("No valid bids. Property remains with the Bank.\n");
    }
}

void declareBankruptcy(Game *game, int playerId) {
    Player *p = &game->players[playerId];
    p->bankrupt = 1;
    p->bankruptRound = game->currentRound;

    printf("\n%s has been declared bankrupt.\n", p->name);
    printf("Remaining assets transferred to the Bank.\n");

    /* Collect all properties to auction */
    int toAuction[BOARD_SIZE];
    int count = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {
        Square *sq = &game->board[i];
        if (sq->ownerId == playerId) {
            sq->ownerId = -1;
            sq->numHouses = 0;
            sq->hasHotel = 0;
            sq->mortgaged = 0;
            sq->loanLocked = 0;
            sq->insurance.active = 0;
            sq->insurance.type = INS_NONE;
            sq->buildingCondition = 100;
            toAuction[count++] = i;
        }
    }

    p->loan.active = 0;
    p->loan.outstandingBalance = 0;

    /* Auction remaining assets (FR-BANKR-02 / FR-AUC-01) */
    for (int i = 0; i < count; i++) {
        triggerAuction(game, toAuction[i]);
    }
}

void printRoundSummary(const Game *game) {
    printf("\n=============================================\n");
    printf("Round %d Summary\n", game->currentRound);
    printf("=============================================\n");

    for (int i = 0; i < NUM_PLAYERS; i++) {
        const Player *p = &game->players[i];
        printf("%s\n", p->name);
        if (p->bankrupt) {
            printf("Status : Bankrupt (Round %d)\n", p->bankruptRound);
        } else {
            int propCount = 0;
            int hotelCount = 0;
            for (int b = 0; b < BOARD_SIZE; b++) {
                if (game->board[b].ownerId == p->id && game->board[b].type == SQ_PROPERTY) {
                    propCount++;
                    if (game->board[b].hasHotel) hotelCount++;
                }
            }
            printf("Cash : LKR %d\n", p->cash);
            printf("Net Worth : LKR %d\n", calculateNetWorth(game, p->id));
            printf("Properties : %d\n", propCount);
            printf("Hotels : %d\n", hotelCount);
            if (p->loan.active) {
                printf("Outstanding Loan : LKR %d\n", p->loan.outstandingBalance);
            } else {
                printf("Outstanding Loan : None\n");
            }
        }
        if (i < NUM_PLAYERS - 1) {
            printf("---------------------------------------------\n");
        }
    }
    printf("=============================================\n");
}

void printMarketConditions(const Game *game) {
    printf("\n=========================================\n");
    printf("Current Market Conditions\n");
    printf("=========================================\n");

    int hasBoom = 0;
    for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) {
        if (game->economy.groupMarket[g].boomRoundsLeft > 0) {
            printf("Market Boom\n-------------\n");
            printf("%s Group (+20%%)\n", getGroupName((PropertyGroup)g));
            printf("Rounds Remaining : %d\n\n", game->economy.groupMarket[g].boomRoundsLeft);
            hasBoom = 1;
            break;
        }
    }
    if (!hasBoom) {
        printf("Market Boom\n-------------\nNone\n\n");
    }

    int hasDecline = 0;
    for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) {
        if (game->economy.groupMarket[g].declineRoundsLeft > 0) {
            printf("Market Decline\n----------------\n");
            printf("%s Group (-15%%)\n", getGroupName((PropertyGroup)g));
            printf("Rounds Remaining : %d\n\n", game->economy.groupMarket[g].declineRoundsLeft);
            hasDecline = 1;
            break;
        }
    }
    if (!hasDecline) {
        printf("Market Decline\n----------------\nNone\n\n");
    }

    if (game->economy.activeRegionalCardId >= 0 && game->economy.regionalCardRoundsLeft > 0) {
        const RegionalCard *rc = &game->regionalDeck[game->economy.activeRegionalCardId];
        printf("Regional Development\n-----------------------\n");
        printf("%s (+%d%%)\n", rc->name, rc->valueModifierPct);
        printf("Rounds Remaining : %d\n\n", game->economy.regionalCardRoundsLeft);
    } else {
        printf("Regional Development\n-----------------------\nNone\n\n");
    }

    printf("Inflation\n------------\n");
    if (game->economy.currentInflationRate >= 0) {
        printf("+%d%%\n\n", game->economy.currentInflationRate);
    } else {
        printf("%d%%\n\n", game->economy.currentInflationRate);
    }

    printf("Current Loan Interest\n-----------------------\n");
    printf("%d%%\n", getPrevailingLoanInterestRate(game));
    printf("=========================================\n");
}

void printGameOverSummary(const Game *game) {
    int winnerId = game->winnerId;
    if (winnerId < 0) {
        /* Determine winner by highest net worth */
        int highestNW = -1;
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (!game->players[i].bankrupt) {
                int nw = calculateNetWorth(game, i);
                if (nw > highestNW) {
                    highestNW = nw;
                    winnerId = i;
                }
            }
        }
    }

    const Player *w = &game->players[winnerId];
    int totalPropVal = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (game->board[i].ownerId == winnerId) {
            totalPropVal += getPropertyMarketValue(game, i) + getBuildingValue(game, i);
        }
    }

    printf("\n=============================================\n");
    printf("GAME OVER\n");
    printf("Winner\n");
    printf("%s\n", w->name);
    printf("Total Cash\n");
    printf("LKR %d\n", w->cash);
    printf("Total Property Value\n");
    printf("LKR %d\n", totalPropVal);
    printf("Outstanding Loans\n");
    if (w->loan.active) {
        printf("LKR %d\n", w->loan.outstandingBalance);
    } else {
        printf("None\n");
    }
    printf("Net Worth\n");
    printf("LKR %d\n", calculateNetWorth(game, winnerId));
    printf("=============================================\n");
}

void runGame(Game *game) {
    /* Startup Banner */
    printf("MONOPOLY-LK Simulation\n\n");
    for (int i = 0; i < NUM_PLAYERS; i++) {
        printf("Player %d : %s\n", i + 1, game->players[i].name);
    }
    printf("\nEach player begins with LKR 30,000.\n\n");

    /* Determine Turn Order */
    determineTurnOrder(game);

    /* Main Game Loop */
    for (game->currentRound = 1; game->currentRound <= MAX_ROUNDS; game->currentRound++) {
        /* Scheduled periodic event checks at start of round */
        updateInflation(game);
        checkDynamicMarket(game);
        checkRegionalCard(game);
        checkGlobalEconomicEvent(game);
        checkGovernmentRegulation(game);
        triggerDisasterCheck(game);

        /* Player turns */
        for (int t = 0; t < NUM_PLAYERS; t++) {
            int pid = game->turnOrder[t];
            Player *p = &game->players[pid];
            if (p->bankrupt) continue;

            /* Turn Step 1: Outstanding penalties / Jail / Maintenance */
            if (p->inJail) {
                p->jailTurns++;
                int d1 = (rand() % 6) + 1;
                int d2 = (rand() % 6) + 1;
                if (d1 == d2) {
                    p->inJail = 0;
                    p->jailTurns = 0;
                    printf("\n%s rolled doubles (%d, %d) and is released from Jail!\n", p->name, d1, d2);
                } else if (p->jailTurns >= 3 || (p->cash >= JAIL_BAIL && p->strategy != STRAT_CONSERVATIVE)) {
                    if (p->cash >= JAIL_BAIL) {
                        p->cash -= JAIL_BAIL;
                        p->inJail = 0;
                        p->jailTurns = 0;
                        printf("\n%s paid LKR %d bail and is released from Jail.\n", p->name, JAIL_BAIL);
                    } else {
                        printf("\n%s remains in Jail (Turn %d).\n", p->name, p->jailTurns);
                        continue;
                    }
                } else {
                    printf("\n%s remains in Jail (Turn %d).\n", p->name, p->jailTurns);
                    continue;
                }
            }

            /* Maintenance & Renovation checks */
            playerPerformTurnMaintenance(game, pid);
            playerPerformTurnRenovation(game, pid);

            /* Turn Step 2: Roll two dice */
            int d1 = (rand() % 6) + 1;
            int d2 = (rand() % 6) + 1;
            int diceRoll = d1 + d2;
            printf("\n%s rolled %d.\n", p->name, diceRoll);

            /* Turn Step 3: Move clockwise */
            int prevPos = p->position;
            int newPos = (prevPos + diceRoll) % BOARD_SIZE;
            p->position = newPos;
            printf("%s moves from Square %d to Square %d.\n", p->name, prevPos, newPos);

            /* Passed or landed on GO */
            if (newPos < prevPos || newPos == 0) {
                p->cash += GO_REWARD;
                printf("%s passed GO.\n", p->name);
                printf("Collected LKR %d.\n", GO_REWARD);
                printf("Current Balance : LKR %d.\n", p->cash);
            }

            /* Turn Step 4 & 5: Landing action */
            Square *sq = &game->board[newPos];

            switch (sq->type) {
                case SQ_START:
                case SQ_SPECIAL:
                    if (newPos == GO_TO_JAIL_SQUARE) {
                        p->inJail = 1;
                        p->jailTurns = 0;
                        p->position = JAIL_SQUARE;
                        printf("%s sent to Jail.\n", p->name);
                    }
                    break;

                case SQ_TAX:
                    if (newPos == INCOME_TAX_SQUARE) {
                        int tax = calculateIncomeTax(game, pid);
                        payPlayerTax(game, pid, tax, "Income Tax");
                    }
                    break;

                case SQ_EVENT:
                    if (newPos == CDF_TAX_SQUARE) {
                        int tax = calculateCDFTax(game, pid);
                        payPlayerTax(game, pid, tax, "Community Development Fund");
                    } else {
                        drawNationalEventCard(game, pid);
                    }
                    break;

                case SQ_INSURANCE:
                    /* Check if player owns any developed property eligible for insurance */
                    for (int b = 0; b < BOARD_SIZE; b++) {
                        if (game->board[b].ownerId == pid && game->board[b].type == SQ_PROPERTY &&
                            (game->board[b].numHouses > 0 || game->board[b].hasHotel) &&
                            !game->board[b].insurance.active) {
                            InsuranceType insType = playerDecideInsurance(game, pid, b);
                            if (insType != INS_NONE) {
                                purchaseInsurance(game, pid, b, insType);
                                break;
                            }
                        }
                    }
                    break;

                case SQ_BANK:
                    /* Commercial Bank landing transaction (FR-BANK-01) */
                    if (p->loan.active) {
                        playerDecideRepayLoan(game, pid);
                    } else {
                        playerDecideTakeLoan(game, pid);
                    }
                    break;

                case SQ_PROPERTY:
                case SQ_RAILWAY:
                case SQ_UTILITY:
                    if (sq->ownerId < 0) {
                        /* Unowned: Step 5 purchase or auction */
                        if (playerDecidePurchase(game, pid, newPos)) {
                            int price = getPropertyMarketValue(game, newPos);
                            p->cash -= price;
                            sq->ownerId = pid;
                            sq->age = 0;
                            printf("\n%s purchased %s for LKR %d.\n", p->name, sq->name, price);
                            printf("Remaining Balance : LKR %d.\n", p->cash);
                        } else {
                            triggerAuction(game, newPos);
                        }
                    } else if (sq->ownerId != pid) {
                        /* Owned by another player: Pay Rent */
                        Player *owner = &game->players[sq->ownerId];
                        int rent = calculateRent(game, newPos, diceRoll);
                        printf("\n%s landed on %s.\n", p->name, sq->name);
                        printf("Rent Paid : LKR %d.\n", rent);
                        printf("Owner : %s.\n", owner->name);
                        handlePlayerPayment(game, pid, sq->ownerId, rent, "Rent");
                    }
                    break;
            }

            if (p->bankrupt) continue;

            /* Turn Step 6: Construct buildings if eligible */
            playerPerformTurnConstruction(game, pid);

            /* Turn Step 7: Complete financial transactions */
            /* Turn Step 8: End turn */
        }

        /* End-of-Round maintenance and processing */
        accrueInterest(game);
        checkLoanDefaults(game);
        checkInsuranceExpiry(game);
        updatePropertyAgeAndCondition(game);
        updateRoundEvents(game);

        /* Print End-of-round Summary */
        printRoundSummary(game);

        /* Print Rule-LK 36 Current Market Conditions */
        printMarketConditions(game);

        /* Check Solvent Players */
        int solventCount = 0;
        int lastSolventId = -1;
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (!game->players[i].bankrupt) {
                solventCount++;
                lastSolventId = i;
            }
        }

        if (solventCount <= 1) {
            game->isGameOver = 1;
            game->winnerId = lastSolventId;
            break;
        }
    }

    /* Print Game Over Summary */
    printGameOverSummary(game);
}
