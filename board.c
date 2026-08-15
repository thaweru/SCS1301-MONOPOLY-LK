#include "board.h"
#include <string.h>

void initBoard(Square board[BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i].index = i;
        board[i].ownerId = -1;
        board[i].mortgaged = 0;
        board[i].loanLocked = 0;
        board[i].numHouses = 0;
        board[i].hasHotel = 0;
        board[i].buildingCondition = 100;
        board[i].consecutiveNeglectedRounds = 0;
        board[i].structuralDamage = 0;
        board[i].age = 0;
        board[i].isDamaged = 0;
        board[i].closedTurns = 0;
        board[i].insurance.type = INS_NONE;
        board[i].insurance.propertyIndex = i;
        board[i].insurance.premium = 0;
        board[i].insurance.compensationRate = 0;
        board[i].insurance.startRound = 0;
        board[i].insurance.duration = 20;
        board[i].insurance.active = 0;
        board[i].insurance.expiryNotified = 0;
        board[i].group = GRP_NONE;
        board[i].region = REG_NONE;
        board[i].basePurchasePrice = 0;
        board[i].baseRental = 0;
        board[i].baseMortgageValue = 0;
        board[i].baseHouseCost = 0;
        board[i].baseHotelCost = 0;
    }

    /* 0: GO */
    board[0].type = SQ_START;
    strncpy(board[0].name, "GO", sizeof(board[0].name));

    /* 1: Pettah */
    board[1].type = SQ_PROPERTY;
    strncpy(board[1].name, "Pettah", sizeof(board[1].name));
    board[1].group = GRP_BROWN;
    board[1].region = REG_COLOMBO_COMMERCIAL;
    board[1].basePurchasePrice = 1200;
    board[1].baseRental = 120;
    board[1].baseHouseCost = 500;
    board[1].baseHotelCost = 2000;
    board[1].baseMortgageValue = 750;

    /* 2: Community Development Fund */
    board[2].type = SQ_EVENT;
    strncpy(board[2].name, "Community Development Fund", sizeof(board[2].name));

    /* 3: Maradana */
    board[3].type = SQ_PROPERTY;
    strncpy(board[3].name, "Maradana", sizeof(board[3].name));
    board[3].group = GRP_BROWN;
    board[3].region = REG_COLOMBO_COMMERCIAL;
    board[3].basePurchasePrice = 1500;
    board[3].baseRental = 150;
    board[3].baseHouseCost = 500;
    board[3].baseHotelCost = 2000;
    board[3].baseMortgageValue = 750;

    /* 4: Income Tax */
    board[4].type = SQ_TAX;
    strncpy(board[4].name, "Income Tax", sizeof(board[4].name));

    /* 5: Colombo Fort Railway Station */
    board[5].type = SQ_RAILWAY;
    strncpy(board[5].name, "Colombo Fort Railway Station", sizeof(board[5].name));
    board[5].region = REG_COLOMBO_COMMERCIAL;
    board[5].basePurchasePrice = 2000;
    board[5].baseRental = 250;
    board[5].baseMortgageValue = 1000;

    /* 6: Bambalapitiya */
    board[6].type = SQ_PROPERTY;
    strncpy(board[6].name, "Bambalapitiya", sizeof(board[6].name));
    board[6].group = GRP_LIGHT_BLUE;
    board[6].region = REG_WESTERN_COASTAL;
    board[6].basePurchasePrice = 2400;
    board[6].baseRental = 240;
    board[6].baseHouseCost = 750;
    board[6].baseHotelCost = 3000;
    board[6].baseMortgageValue = 1250;

    /* 7: National Event Card */
    board[7].type = SQ_EVENT;
    strncpy(board[7].name, "National Event Card", sizeof(board[7].name));

    /* 8: Wellawatte */
    board[8].type = SQ_PROPERTY;
    strncpy(board[8].name, "Wellawatte", sizeof(board[8].name));
    board[8].group = GRP_LIGHT_BLUE;
    board[8].region = REG_WESTERN_COASTAL;
    board[8].basePurchasePrice = 2500;
    board[8].baseRental = 250;
    board[8].baseHouseCost = 750;
    board[8].baseHotelCost = 3000;
    board[8].baseMortgageValue = 1250;

    /* 9: Mount Lavinia */
    board[9].type = SQ_PROPERTY;
    strncpy(board[9].name, "Mount Lavinia", sizeof(board[9].name));
    board[9].group = GRP_LIGHT_BLUE;
    board[9].region = REG_WESTERN_COASTAL;
    board[9].basePurchasePrice = 2800;
    board[9].baseRental = 280;
    board[9].baseHouseCost = 750;
    board[9].baseHotelCost = 3000;
    board[9].baseMortgageValue = 1250;

    /* 10: Jail / Just Visiting */
    board[10].type = SQ_SPECIAL;
    strncpy(board[10].name, "Jail / Just Visiting", sizeof(board[10].name));

    /* 11: Nugegoda */
    board[11].type = SQ_PROPERTY;
    strncpy(board[11].name, "Nugegoda", sizeof(board[11].name));
    board[11].group = GRP_PINK;
    board[11].region = REG_WESTERN_RESIDENTIAL;
    board[11].basePurchasePrice = 3200;
    board[11].baseRental = 320;
    board[11].baseHouseCost = 1000;
    board[11].baseHotelCost = 4000;
    board[11].baseMortgageValue = 1750;

    /* 12: Ceylon Electricity Board */
    board[12].type = SQ_UTILITY;
    strncpy(board[12].name, "Ceylon Electricity Board", sizeof(board[12].name));
    board[12].basePurchasePrice = 2500;
    board[12].baseMortgageValue = 1250;

    /* 13: Maharagama */
    board[13].type = SQ_PROPERTY;
    strncpy(board[13].name, "Maharagama", sizeof(board[13].name));
    board[13].group = GRP_PINK;
    board[13].region = REG_WESTERN_RESIDENTIAL;
    board[13].basePurchasePrice = 3500;
    board[13].baseRental = 350;
    board[13].baseHouseCost = 1000;
    board[13].baseHotelCost = 4000;
    board[13].baseMortgageValue = 1750;

    /* 14: Kottawa */
    board[14].type = SQ_PROPERTY;
    strncpy(board[14].name, "Kottawa", sizeof(board[14].name));
    board[14].group = GRP_PINK;
    board[14].region = REG_WESTERN_RESIDENTIAL;
    board[14].basePurchasePrice = 3800;
    board[14].baseRental = 380;
    board[14].baseHouseCost = 1000;
    board[14].baseHotelCost = 4000;
    board[14].baseMortgageValue = 1750;

    /* 15: Kandy Railway Station */
    board[15].type = SQ_RAILWAY;
    strncpy(board[15].name, "Kandy Railway Station", sizeof(board[15].name));
    board[15].region = REG_CENTRAL_HILL;
    board[15].basePurchasePrice = 2000;
    board[15].baseRental = 250;
    board[15].baseMortgageValue = 1000;

    /* 16: Negombo */
    board[16].type = SQ_PROPERTY;
    strncpy(board[16].name, "Negombo", sizeof(board[16].name));
    board[16].group = GRP_ORANGE;
    board[16].region = REG_WESTERN_AIRPORT;
    board[16].basePurchasePrice = 4200;
    board[16].baseRental = 420;
    board[16].baseHouseCost = 1250;
    board[16].baseHotelCost = 5000;
    board[16].baseMortgageValue = 2250;

    /* 17: Sri Lanka Insurance */
    board[17].type = SQ_INSURANCE;
    strncpy(board[17].name, "Sri Lanka Insurance", sizeof(board[17].name));

    /* 18: Katunayake */
    board[18].type = SQ_PROPERTY;
    strncpy(board[18].name, "Katunayake", sizeof(board[18].name));
    board[18].group = GRP_ORANGE;
    board[18].region = REG_WESTERN_AIRPORT;
    board[18].basePurchasePrice = 4500;
    board[18].baseRental = 450;
    board[18].baseHouseCost = 1250;
    board[18].baseHotelCost = 5000;
    board[18].baseMortgageValue = 2250;

    /* 19: Ja-Ela */
    board[19].type = SQ_PROPERTY;
    strncpy(board[19].name, "Ja-Ela", sizeof(board[19].name));
    board[19].group = GRP_ORANGE;
    board[19].region = REG_WESTERN_AIRPORT;
    board[19].basePurchasePrice = 4800;
    board[19].baseRental = 480;
    board[19].baseHouseCost = 1250;
    board[19].baseHotelCost = 5000;
    board[19].baseMortgageValue = 2250;

    /* 20: Free Parking */
    board[20].type = SQ_SPECIAL;
    strncpy(board[20].name, "Free Parking", sizeof(board[20].name));

    /* 21: Kandy City */
    board[21].type = SQ_PROPERTY;
    strncpy(board[21].name, "Kandy City", sizeof(board[21].name));
    board[21].group = GRP_RED;
    board[21].region = REG_CENTRAL_HILL;
    board[21].basePurchasePrice = 5200;
    board[21].baseRental = 520;
    board[21].baseHouseCost = 1500;
    board[21].baseHotelCost = 6000;
    board[21].baseMortgageValue = 2750;

    /* 22: National Event Card */
    board[22].type = SQ_EVENT;
    strncpy(board[22].name, "National Event Card", sizeof(board[22].name));

    /* 23: Peradeniya */
    board[23].type = SQ_PROPERTY;
    strncpy(board[23].name, "Peradeniya", sizeof(board[23].name));
    board[23].group = GRP_RED;
    board[23].region = REG_CENTRAL_HILL;
    board[23].basePurchasePrice = 5500;
    board[23].baseRental = 550;
    board[23].baseHouseCost = 1500;
    board[23].baseHotelCost = 6000;
    board[23].baseMortgageValue = 2750;

    /* 24: Katugastota */
    board[24].type = SQ_PROPERTY;
    strncpy(board[24].name, "Katugastota", sizeof(board[24].name));
    board[24].group = GRP_RED;
    board[24].region = REG_CENTRAL_HILL;
    board[24].basePurchasePrice = 5800;
    board[24].baseRental = 580;
    board[24].baseHouseCost = 1500;
    board[24].baseHotelCost = 6000;
    board[24].baseMortgageValue = 2750;

    /* 25: Galle Railway Station */
    board[25].type = SQ_RAILWAY;
    strncpy(board[25].name, "Galle Railway Station", sizeof(board[25].name));
    board[25].region = REG_SOUTHERN_PROVINCE;
    board[25].basePurchasePrice = 2000;
    board[25].baseRental = 250;
    board[25].baseMortgageValue = 1000;

    /* 26: Galle Fort */
    board[26].type = SQ_PROPERTY;
    strncpy(board[26].name, "Galle Fort", sizeof(board[26].name));
    board[26].group = GRP_YELLOW;
    board[26].region = REG_SOUTHERN_PROVINCE;
    board[26].basePurchasePrice = 4500;
    board[26].baseRental = 750;
    board[26].baseHouseCost = 2000;
    board[26].baseHotelCost = 8000;
    board[26].baseMortgageValue = 3250;

    /* 27: Unawatuna */
    board[27].type = SQ_PROPERTY;
    strncpy(board[27].name, "Unawatuna", sizeof(board[27].name));
    board[27].group = GRP_YELLOW;
    board[27].region = REG_SOUTHERN_PROVINCE;
    board[27].basePurchasePrice = 6500;
    board[27].baseRental = 650;
    board[27].baseHouseCost = 2000;
    board[27].baseHotelCost = 8000;
    board[27].baseMortgageValue = 3250;

    /* 28: National Water Supply and Drainage Board */
    board[28].type = SQ_UTILITY;
    strncpy(board[28].name, "National Water Supply and Drainage Board", sizeof(board[28].name));
    board[28].basePurchasePrice = 2500;
    board[28].baseMortgageValue = 1250;

    /* 29: Hikkaduwa */
    board[29].type = SQ_PROPERTY;
    strncpy(board[29].name, "Hikkaduwa", sizeof(board[29].name));
    board[29].group = GRP_YELLOW;
    board[29].region = REG_SOUTHERN_PROVINCE;
    board[29].basePurchasePrice = 6800;
    board[29].baseRental = 680;
    board[29].baseHouseCost = 2000;
    board[29].baseHotelCost = 8000;
    board[29].baseMortgageValue = 3250;

    /* 30: Go To Jail */
    board[30].type = SQ_SPECIAL;
    strncpy(board[30].name, "Go To Jail", sizeof(board[30].name));

    /* 31: Jaffna Town */
    board[31].type = SQ_PROPERTY;
    strncpy(board[31].name, "Jaffna Town", sizeof(board[31].name));
    board[31].group = GRP_GREEN;
    board[31].region = REG_NORTHERN_PROVINCE;
    board[31].basePurchasePrice = 7500;
    board[31].baseRental = 750;
    board[31].baseHouseCost = 2500;
    board[31].baseHotelCost = 10000;
    board[31].baseMortgageValue = 4000;

    /* 32: Nallur */
    board[32].type = SQ_PROPERTY;
    strncpy(board[32].name, "Nallur", sizeof(board[32].name));
    board[32].group = GRP_GREEN;
    board[32].region = REG_NORTHERN_PROVINCE;
    board[32].basePurchasePrice = 8000;
    board[32].baseRental = 800;
    board[32].baseHouseCost = 2500;
    board[32].baseHotelCost = 10000;
    board[32].baseMortgageValue = 4000;

    /* 33: Ceylinco Insurance */
    board[33].type = SQ_INSURANCE;
    strncpy(board[33].name, "Ceylinco Insurance", sizeof(board[33].name));

    /* 34: Trincomalee */
    board[34].type = SQ_PROPERTY;
    strncpy(board[34].name, "Trincomalee", sizeof(board[34].name));
    board[34].group = GRP_GREEN;
    board[34].region = REG_EASTERN_PROVINCE;
    board[34].basePurchasePrice = 8500;
    board[34].baseRental = 850;
    board[34].baseHouseCost = 2500;
    board[34].baseHotelCost = 10000;
    board[34].baseMortgageValue = 4000;

    /* 35: Jaffna Railway Station */
    board[35].type = SQ_RAILWAY;
    strncpy(board[35].name, "Jaffna Railway Station", sizeof(board[35].name));
    board[35].region = REG_NORTHERN_PROVINCE;
    board[35].basePurchasePrice = 2000;
    board[35].baseRental = 250;
    board[35].baseMortgageValue = 1000;

    /* 36: National Event Card */
    board[36].type = SQ_EVENT;
    strncpy(board[36].name, "National Event Card", sizeof(board[36].name));

    /* 37: Nuwara Eliya */
    board[37].type = SQ_PROPERTY;
    strncpy(board[37].name, "Nuwara Eliya", sizeof(board[37].name));
    board[37].group = GRP_DARK_BLUE;
    board[37].region = REG_CENTRAL_HILL;
    board[37].basePurchasePrice = 9500;
    board[37].baseRental = 950;
    board[37].baseHouseCost = 3000;
    board[37].baseHotelCost = 12000;
    board[37].baseMortgageValue = 5000;

    /* 38: Bank of Ceylon */
    board[38].type = SQ_BANK;
    strncpy(board[38].name, "Bank of Ceylon", sizeof(board[38].name));

    /* 39: Galle Face */
    board[39].type = SQ_PROPERTY;
    strncpy(board[39].name, "Galle Face", sizeof(board[39].name));
    board[39].group = GRP_DARK_BLUE;
    board[39].region = REG_PRIME_LUXURY;
    board[39].basePurchasePrice = 10000;
    board[39].baseRental = 1000;
    board[39].baseHouseCost = 3000;
    board[39].baseHotelCost = 12000;
    board[39].baseMortgageValue = 5000;
}

int countPropertiesInGroup(PropertyGroup grp) {
    if (grp == GRP_BROWN || grp == GRP_DARK_BLUE) {
        return 2;
    }
    return 3;
}

int countOwnedInGroup(const Square board[BOARD_SIZE], PropertyGroup grp, int playerId) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i].type == SQ_PROPERTY && board[i].group == grp && board[i].ownerId == playerId) {
            count++;
        }
    }
    return count;
}

int hasMonopoly(const Square board[BOARD_SIZE], PropertyGroup grp, int playerId) {
    if (grp == GRP_NONE || playerId < 0) return 0;
    return countOwnedInGroup(board, grp, playerId) == countPropertiesInGroup(grp);
}

int countStationsOwned(const Square board[BOARD_SIZE], int playerId) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i].type == SQ_RAILWAY && board[i].ownerId == playerId && !board[i].mortgaged) {
            count++;
        }
    }
    return count;
}

int countUtilitiesOwned(const Square board[BOARD_SIZE], int playerId) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i].type == SQ_UTILITY && board[i].ownerId == playerId && !board[i].mortgaged) {
            count++;
        }
    }
    return count;
}

const char* getGroupName(PropertyGroup grp) {
    switch (grp) {
        case GRP_BROWN: return "Brown";
        case GRP_LIGHT_BLUE: return "Light Blue";
        case GRP_PINK: return "Pink";
        case GRP_ORANGE: return "Orange";
        case GRP_RED: return "Red";
        case GRP_YELLOW: return "Yellow";
        case GRP_GREEN: return "Green";
        case GRP_DARK_BLUE: return "Dark Blue";
        default: return "None";
    }
}

const char* getRegionName(RegionType reg) {
    switch (reg) {
        case REG_COLOMBO_COMMERCIAL: return "Colombo Commercial";
        case REG_WESTERN_COASTAL: return "Western Coastal";
        case REG_WESTERN_RESIDENTIAL: return "Western Residential";
        case REG_WESTERN_AIRPORT: return "Western Airport";
        case REG_CENTRAL_HILL: return "Central Hill Country";
        case REG_SOUTHERN_PROVINCE: return "Southern Province";
        case REG_NORTHERN_PROVINCE: return "Northern Province";
        case REG_EASTERN_PROVINCE: return "Eastern Province";
        case REG_PRIME_LUXURY: return "Prime Luxury";
        default: return "General";
    }
}
