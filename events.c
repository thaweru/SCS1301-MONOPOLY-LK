#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initEvents(Game *game) {
    /* Economy state */
    game->economy.currentInflationRate = 0;
    game->economy.cumulativeInflationMultiplier = 1.0;
    game->economy.currentBaseLoanInterest = 8;
    game->economy.activeRegionalCardId = -1;
    game->economy.regionalCardRoundsLeft = 0;
    game->economy.activeGlobalEvent = GLOB_NONE;
    game->economy.globalEventRoundsLeft = 0;
    game->economy.activeGovRegulation = GOV_NONE;
    game->economy.govRegulationRoundsLeft = 0;
    game->economy.lastBoomGroup = -1;
    game->economy.lastDeclineGroup = -1;

    for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) {
        game->economy.groupMarket[g].boomRoundsLeft = 0;
        game->economy.groupMarket[g].declineRoundsLeft = 0;
        game->economy.groupMarket[g].cooldownRoundsLeft = 0;
    }

    /* 20 National Cards */
    for (int i = 0; i < NUM_NATIONAL_CARDS; i++) {
        game->nationalDeck[i] = i;
    }
    game->nationalDeckTop = 0;

    /* 12 Regional Development Cards */
    const char *regNames[12] = {
        "Northern Development Programme",
        "Southern Tourism Corridor",
        "Western Megapolis Project",
        "Colombo Financial City",
        "Hill Country Tea Tourism",
        "Airport Expressway Expansion",
        "Coastal Zone Conservation",
        "Cultural Triangle Restoration",
        "Jaffna Peninsula Tech Hub",
        "Southern Expressway Extension",
        "Port City Urban Development",
        "East Coast Tourism Promotion"
    };

    const char *regDescs[12] = {
        "Northern properties increase in value and rent by 30%.",
        "Southern Province properties increase rental income by 25% and value by 20%.",
        "Western Residential properties value +20%, construction cost -15%.",
        "Colombo Commercial properties increase value by 25% and rent by 20%.",
        "Central Hill Country properties increase rent by 25%.",
        "Western Airport properties increase value by 20% and rent by 15%.",
        "Coastal properties construction cost +25%, value +10%.",
        "Cultural Triangle properties value +15%, rent +20%.",
        "Jaffna Town & Nallur value +25%, construction cost -20%.",
        "Southern properties value +20%, rent +15%.",
        "Port City & Prime Luxury properties value +30%, rent +25%.",
        "Trincomalee rent +30%, value +20%."
    };

    RegionType regRegions[12] = {
        REG_NORTHERN_PROVINCE,
        REG_SOUTHERN_PROVINCE,
        REG_WESTERN_RESIDENTIAL,
        REG_COLOMBO_COMMERCIAL,
        REG_CENTRAL_HILL,
        REG_WESTERN_AIRPORT,
        REG_WESTERN_COASTAL,
        REG_CENTRAL_HILL,
        REG_NORTHERN_PROVINCE,
        REG_SOUTHERN_PROVINCE,
        REG_PRIME_LUXURY,
        REG_EASTERN_PROVINCE
    };

    int valMods[12] = {30, 20, 20, 25, 0, 20, 10, 15, 25, 20, 30, 20};
    int rentMods[12] = {30, 25, 0, 20, 25, 15, 0, 20, 0, 15, 25, 30};
    int constrMods[12] = {0, 0, -15, 0, 0, 0, 25, 0, -20, 0, 0, 0};

    for (int i = 0; i < NUM_REGIONAL_CARDS; i++) {
        game->regionalDeck[i].id = i;
        strncpy(game->regionalDeck[i].name, regNames[i], sizeof(game->regionalDeck[i].name));
        strncpy(game->regionalDeck[i].description, regDescs[i], sizeof(game->regionalDeck[i].description));
        game->regionalDeck[i].targetRegion = regRegions[i];
        game->regionalDeck[i].targetGroup = GRP_NONE;
        game->regionalDeck[i].valueModifierPct = valMods[i];
        game->regionalDeck[i].rentModifierPct = rentMods[i];
        game->regionalDeck[i].constrModifierPct = constrMods[i];
    }

    /* 8 Global Economic Events (Rule-LK 18) */
    const char *globNames[8] = {
        "Tourism Boom",
        "Fuel Crisis",
        "Heavy Monsoon",
        "Economic Recession",
        "Stock Market Boom",
        "Government Housing Programme",
        "Foreign Investment",
        "Political Unrest"
    };

    const char *globDescs[8] = {
        "Southern Province properties increase in value by 15%.",
        "Railway rent doubles and transport costs increase by 20%.",
        "Heavy rain and floods reduce non-hotel rental income by 15%.",
        "Property values and rental incomes decrease by 15%.",
        "Stock market surges, increasing all property values by 15%.",
        "Government subsidises construction costs by 25%.",
        "Foreign direct investment boosts commercial property values by 25%.",
        "Civil unrest decreases property rental income by 20%."
    };

    for (int i = 0; i < NUM_GLOBAL_EVENTS; i++) {
        game->globalEvents[i].type = (GlobalEventType)i;
        strncpy(game->globalEvents[i].name, globNames[i], sizeof(game->globalEvents[i].name));
        strncpy(game->globalEvents[i].description, globDescs[i], sizeof(game->globalEvents[i].description));
    }

    /* 8 Government Regulations (Rule-LK 24) */
    const char *govNames[8] = {
        "Increase Property Tax",
        "Reduce Loan Interest",
        "Housing Subsidy Introduced",
        "Luxury Property Tax",
        "Railway Modernization",
        "Electricity Tariff Revision",
        "Insurance Regulation",
        "Anti-Speculation Act"
    };

    const char *govDescs[8] = {
        "Property tax increased by 5%.",
        "Loan interest rates reduced by 2%.",
        "Construction costs reduced by 30%.",
        "Luxury and prime properties incur additional tax.",
        "Railway station rental income increased by 50%.",
        "Utility tariffs and rental multipliers revised.",
        "Insurance premiums discounted by 20%.",
        "Undeveloped property holdings capped at 3 per player."
    };

    for (int i = 0; i < NUM_GOV_REGULATIONS; i++) {
        game->govRegulations[i].type = (GovRegulationType)i;
        strncpy(game->govRegulations[i].name, govNames[i], sizeof(game->govRegulations[i].name));
        strncpy(game->govRegulations[i].description, govDescs[i], sizeof(game->govRegulations[i].description));
    }
}

/* Dynamic Property Market (FR-MKT-01..05) */
void checkDynamicMarket(Game *game) {
    if (game->currentRound % 10 != 0) return;

    /* Select eligible groups for Boom and Decline */
    int eligibleBoom[NUM_PROPERTY_GROUPS];
    int boomCount = 0;
    for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) {
        if (game->economy.groupMarket[g].cooldownRoundsLeft <= 0 && g != game->economy.lastBoomGroup) {
            eligibleBoom[boomCount++] = g;
        }
    }
    if (boomCount == 0) {
        for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) eligibleBoom[boomCount++] = g;
    }

    int boomGroup = eligibleBoom[rand() % boomCount];

    int eligibleDecline[NUM_PROPERTY_GROUPS];
    int declineCount = 0;
    for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) {
        if (g != boomGroup && game->economy.groupMarket[g].cooldownRoundsLeft <= 0 && g != game->economy.lastDeclineGroup) {
            eligibleDecline[declineCount++] = g;
        }
    }
    if (declineCount == 0) {
        for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) {
            if (g != boomGroup) eligibleDecline[declineCount++] = g;
        }
    }

    int declineGroup = eligibleDecline[rand() % declineCount];

    game->economy.groupMarket[boomGroup].boomRoundsLeft = 10;
    game->economy.groupMarket[boomGroup].cooldownRoundsLeft = 30;
    game->economy.lastBoomGroup = boomGroup;

    game->economy.groupMarket[declineGroup].declineRoundsLeft = 10;
    game->economy.groupMarket[declineGroup].cooldownRoundsLeft = 30;
    game->economy.lastDeclineGroup = declineGroup;

    printf("\n--- Dynamic Property Market Review ---\n");
    printf("Market Boom : %s Group (+20%% Value, +25%% Rent for 10 rounds)\n", getGroupName((PropertyGroup)boomGroup));
    printf("Market Decline : %s Group (-15%% Value, -20%% Rent for 10 rounds)\n", getGroupName((PropertyGroup)declineGroup));
}

/* Regional Development Card (FR-REG-01..03) */
void checkRegionalCard(Game *game) {
    if (game->currentRound % 15 != 0) return;

    int cardId = rand() % NUM_REGIONAL_CARDS;
    game->economy.activeRegionalCardId = cardId;
    game->economy.regionalCardRoundsLeft = 15;

    printf("\nRegional Development Card Drawn\n");
    printf("%s\n", game->regionalDeck[cardId].name);
    printf("%s\n", game->regionalDeck[cardId].description);
}

/* Global Economic Event (FR-ECO-01..02) */
void checkGlobalEconomicEvent(Game *game) {
    if (game->currentRound % 15 != 0) return;

    int eventIdx = rand() % NUM_GLOBAL_EVENTS;
    game->economy.activeGlobalEvent = (GlobalEventType)eventIdx;
    game->economy.globalEventRoundsLeft = 15;

    printf("\nEconomic Event\n");
    printf("%s\n", game->globalEvents[eventIdx].name);
    printf("%s\n", game->globalEvents[eventIdx].description);
}

/* Government Regulation (FR-GOV-01..02) */
void checkGovernmentRegulation(Game *game) {
    if (game->currentRound % 20 != 0) return;

    int regIdx = rand() % NUM_GOV_REGULATIONS;
    game->economy.activeGovRegulation = (GovRegulationType)regIdx;
    game->economy.govRegulationRoundsLeft = 20;

    printf("\nGovernment Regulation\n");
    printf("%s\n", game->govRegulations[regIdx].name);
    printf("%s\n", game->govRegulations[regIdx].description);
}

/* Update Round Events Timers at end of round */
void updateRoundEvents(Game *game) {
    for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) {
        if (game->economy.groupMarket[g].boomRoundsLeft > 0) {
            game->economy.groupMarket[g].boomRoundsLeft--;
        }
        if (game->economy.groupMarket[g].declineRoundsLeft > 0) {
            game->economy.groupMarket[g].declineRoundsLeft--;
        }
        if (game->economy.groupMarket[g].cooldownRoundsLeft > 0) {
            game->economy.groupMarket[g].cooldownRoundsLeft--;
        }
    }

    if (game->economy.regionalCardRoundsLeft > 0) {
        game->economy.regionalCardRoundsLeft--;
        if (game->economy.regionalCardRoundsLeft == 0) {
            game->economy.activeRegionalCardId = -1;
        }
    }

    if (game->economy.globalEventRoundsLeft > 0) {
        game->economy.globalEventRoundsLeft--;
        if (game->economy.globalEventRoundsLeft == 0) {
            game->economy.activeGlobalEvent = GLOB_NONE;
        }
    }

    if (game->economy.govRegulationRoundsLeft > 0) {
        game->economy.govRegulationRoundsLeft--;
        if (game->economy.govRegulationRoundsLeft == 0) {
            game->economy.activeGovRegulation = GOV_NONE;
        }
    }

    for (int i = 0; i < NUM_PLAYERS; i++) {
        Player *p = &game->players[i];
        if (p->nationalCardRoundsLeft > 0) {
            p->nationalCardRoundsLeft--;
            if (p->nationalCardRoundsLeft == 0) {
                p->activeNationalCard = -1;
            }
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (game->board[i].closedTurns > 0) {
            game->board[i].closedTurns--;
        }
    }
}

/* Draw National Event Card on landing on squares 7, 22, 36 (Appendix A) */
void drawNationalEventCard(Game *game, int playerId) {
    Player *p = &game->players[playerId];
    int cardId = game->nationalDeck[game->nationalDeckTop];
    game->nationalDeckTop = (game->nationalDeckTop + 1) % NUM_NATIONAL_CARDS;

    const char *cardNames[20] = {
        "Tourism Hype", "Fuel Shortage", "Heavy Floods", "Political Rally",
        "Stock Market Rise", "Economic Downturn", "Housing Subsidy", "Interest Rate Cut",
        "Interest Rate Increase", "Tax Amnesty", "Power Failure", "Foreign Funding",
        "Port Expansion", "Festival Season", "Labour Strike", "Insurance Discount",
        "Property Revaluation", "Currency Depreciation", "Government Grant", "National Disaster"
    };

    const char *cardDescs[20] = {
        "Hotels earn double rent for 5 rounds",
        "Railway rent doubles for 5 rounds",
        "Random coastal property damaged",
        "One random property closed for 2 rounds",
        "All property values increase by 10%",
        "Property values decrease by 15%",
        "House construction cost reduced by 30%",
        "Loan interest reduced by 2%",
        "Loan interest increased by 2%",
        "Each player receives LKR 2,000",
        "Utility income halved for 3 rounds",
        "Commercial property values increase by 15%",
        "Railway station values increase by 20%",
        "Hotels receive 50% additional rent",
        "Construction suspended for 2 rounds",
        "Premiums reduced by 20%",
        "Random property group appreciates by 15%",
        "Construction costs increase by 10%",
        "Random player receives LKR 5,000",
        "Random developed property damaged"
    };

    printf("\n%s drew National Event Card : %s\n", p->name, cardNames[cardId]);
    printf("Effect : %s\n", cardDescs[cardId]);

    p->activeNationalCard = cardId;
    p->nationalCardRoundsLeft = 15;

    /* Instantaneous or special effect processing */
    switch (cardId) {
        case 9: /* Tax Amnesty */
            for (int i = 0; i < NUM_PLAYERS; i++) {
                if (!game->players[i].bankrupt) {
                    game->players[i].cash += 2000;
                    printf("%s received Tax Amnesty grant of LKR 2,000.\n", game->players[i].name);
                }
            }
            break;
        case 18: /* Government Grant */
            {
                int rIdx = rand() % NUM_PLAYERS;
                while (game->players[rIdx].bankrupt) {
                    rIdx = rand() % NUM_PLAYERS;
                }
                game->players[rIdx].cash += 5000;
                printf("%s received Government Grant of LKR 5,000.\n", game->players[rIdx].name);
            }
            break;
        case 2: /* Heavy Floods */
            {
                int coastal[10];
                int cCount = 0;
                for (int i = 0; i < BOARD_SIZE; i++) {
                    if (game->board[i].region == REG_WESTERN_COASTAL || game->board[i].region == REG_SOUTHERN_PROVINCE) {
                        if (game->board[i].ownerId >= 0) {
                            coastal[cCount++] = i;
                        }
                    }
                }
                if (cCount > 0) {
                    int hit = coastal[rand() % cCount];
                    game->board[hit].isDamaged = 1;
                    printf("%s damaged by Heavy Floods!\n", game->board[hit].name);
                }
            }
            break;
        case 3: /* Political Rally */
            {
                int props[BOARD_SIZE];
                int pCount = 0;
                for (int i = 0; i < BOARD_SIZE; i++) {
                    if (game->board[i].type == SQ_PROPERTY && game->board[i].ownerId >= 0) {
                        props[pCount++] = i;
                    }
                }
                if (pCount > 0) {
                    int hit = props[rand() % pCount];
                    game->board[hit].closedTurns = 2;
                    printf("%s temporarily closed for 2 rounds due to Political Rally.\n", game->board[hit].name);
                }
            }
            break;
        default:
            break;
    }
}
