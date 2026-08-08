#include <stdio.h>
typedef int plyr;

typedef enum sqrType{
	START,
	PROPERTY,
	RAILWAY,
	UTILITY,
	EVENT,
	SPECIAL,
	INSURANCE,
	BANK,
    TAX,
	EMPTY = -1
} sqrTyp;

typedef enum propertyGroup{
	BROWN,
	LIGHT_BLUE,
	PINK,
	ORANGE,
	RED,
	YELLOW,
	GREEN,
	DARK_BLUE,
	JAIL = 10,
	GO_TO_JAIL,
	EVENT_CARD,
	CD_FUND,
	NONE = -1
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

sqrgrp property_group(char *str){
    if (strcmp(str, "Brown") == 0) return BROWN;
	if (strcmp(str, "Light Blue") == 0) return LIGHT_BLUE;
	if (strcmp(str, "Pink") == 0) return PINK;
	if (strcmp(str, "Orange") == 0) return ORANGE;
	if (strcmp(str, "Red") == 0) return RED;
	if (strcmp(str, "Yellow") == 0) return YELLOW;
	if (strcmp(str, "Green") == 0) return GREEN;
	if (strcmp(str, "Dark Blue") == 0) return DARK_BLUE;
	if (strcmp(str, "Jail / Just Visiting") == 0) return JAIL;
	if (strcmp(str, "Go To Jail") == 0) return GO_TO_JAIL;
	if (strcmp(str, "National Event Card") == 0) return EVENT_CARD;
	if (strcmp(str, "Community Development Fund") == 0) return CD_FUND;
	return NONE;
}

sqrTyp square_type(char *str){
	if (strcmp(str, "START") == 0) return START;
	if (strcmp(str, "PROPERTY") == 0) return PROPERTY;
	if (strcmp(str, "RAILWAY") == 0) return RAILWAY;
	if (strcmp(str, "UTILITY") == 0) return UTILITY;
	if (strcmp(str, "EVENT") == 0) return EVENT;
	if (strcmp(str, "SPECIAL") == 0) return SPECIAL;
	if (strcmp(str, "INSURANCE") == 0) return INSURANCE;
	if (strcmp(str, "BANK") == 0) return BANK;
	if (strcmp(str, "TAX") == 0) return TAX;
	return EMPTY;
}

int main(){
	bd bord;
	FILE *fp = fopen("square_info.csv", "r");
	//char li[128];
	char typ[16];
	char name[48];
	fscanf(fp, " %*[^\n]");
	for (int i=0; i < 40; i++){
		fscanf(fp, " %[^,],%[^,],", typ, name);
		bord.s[i].type = square_type(typ);
		strcpy(bord.s[i].name, name);
		bord.s[i].owner = NULL;
        bord.s[i].group = NONE;
		switch (bord.s[i].type){
			case PROPERTY:
				fscanf(fp,"%d,%d,%15[^,],%d,%d\n",
				&bord.s[i].buyPrice, 
				&bord.s[i].baseRent, 
				typ, 
				&bord.s[i].houseCost, 
				&bord.s[i].hotelCost
				);
				bord.s[i].group = property_group(typ);
				//puts(name);
				break;
			case RAILWAY:
				fscanf(fp,"%d,%*[^\n]\n", &bord.s[i].buyPrice);
				bord.s[i].group = NONE;//property_group(typ);
				break;
			case UTILITY:
				fscanf(fp,"%d,%*[^\n]\n", &bord.s[i].buyPrice);
				bord.s[i].group = NONE;//property_group(typ);
				break;
			case SPECIAL:
				fscanf(fp, "%*[^\n]\n");
				bord.s[i].group = property_group(name);
				//puts(typ);
				break;
			default:
				fscanf(fp, "%*[^\n]\n");
				break;
		}
		printf("%i\n\t%s\n\ttype:%i\n\tgroup:%i\n",
		i, bord.s[i].name, bord.s[i].type, bord.s[i].group);
	}
//	bord.s[0] = {0, "GO"};
//	return bord;
}
