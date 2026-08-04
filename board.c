#include "types.h"
#include <stdlib.h>
#include <time.h>

sqrgrp property_group(char *str){
	if (strcmp(str, "Brown")) return BROWN;
	if (strcmp(str, "Light Blue")) return LIGHT_BLUE;
	if (strcmp(str, "Pink")) return PINK;
	if (strcmp(str, "Orange")) return ORANGE;
	if (strcmp(str, "Red")) return RED;
	if (strcmp(str, "Yellow")) return YELLOW;
	if (strcmp(str, "Green")) return GREEN;
	if (strcmp(str, "Dark Blue")) return DARK_BLUE;
	return NONE;
}

sqrTyp square_type(char *str){
	if (strcmp(str, "START") == 0) return START;
	if (strcmp(str, "PROPERTY") == 0) return PROPERTY;
	if (strcmp(str, "RAILWAY") == 0) return RAILWAY;
	if (strcmp(str, "UTILITY") == 0) return UTILITY;
	if (strcmp(str, "EVENT") == 0) return EVENT;
	if (strcmp(str, "SPECIAL") == 0) return SPECIAL;
	if (strcmp(str, "BANK") == 0) return BANK;
	return EMPTY;
}

bd init_game(int n, char **arr){
	unsigned int seed;
	if (n == 2){
		char *str = arr[1];
		seed = strtol(str, NULL, 10) % UINT_MAX;
	}else{
		seed = time(NULL);
	}
	srand(seed);
	printf("Seed : %u\n", seed);

	bd bord;
	FILE *fp = fopen("square_info.csv", "r");
	char li[128];
	char typ[12];
	char name[42];
	fscanf(fp, " %[^\n]", li);
	for (int i=0; i < 40; i++){
		fscanf(fp, " %[^,],%[^,],", typ, name);
		strcpy(bord.s[i].name, name);
		bord.s[i].type = square_type(typ);
		switch (bord.s[i].type){	
			case PROPERTY:
			fscanf(fp, " %i,%i,%[^,],", 
				&bord.s[i].buyPrice, 
				&bord.s[i].baseRent, 
				name 
				);
			bord.s[i].group = property_group(name);
			default:
			fscanf(fp, "%[^\n]\n", li);
			//puts("test if read");
		}
	puts(bord.s[i].name);
	printf("BP:%i\tBR:%i, %i\n", 
			bord.s[i].buyPrice, 
			bord.s[i].baseRent, 
			bord.s[i].group
		);
		fscanf(fp, "%[^\n]\n", li);
		puts(li);
	}
//	bord.s[0] = {0, "GO"};
	return bord;
}

int dice(char *duble){
	int a = rand() % 6 + 1;
	int b = rand() % 6 + 1;
	if (a == b){
		*(duble) = 1;
	}else{
		*(duble) = 0;
	}
	return a+b;
}

void start_game(){
}
