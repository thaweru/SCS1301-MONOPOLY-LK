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
	//char li[128];
	char typ[16];
	char name[48];
	fscanf(fp, " %*[^\n]");
	for (int i=0; i < 40; i++){
		fscanf(fp, " %[^,],%[^,],", typ, name);
		bord.s[i].type = square_type(typ);
		strcpy(bord.s[i].name, name);
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
			default:
				fscanf(fp, "%*[^\n]\n");
				//puts(typ);
				break;
		}
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

void start_game(plyr *plyrs, int n, int cash){
	struct keyval{
		plyr a;
		int k;
		char same;
	} p[n], tmp;
	char d;
	for (int i=0; i < n; i++){
		p[i].a = spawn_player(i);
		p[i].a.cash = cash;
		p[i].k = dice(&d);
		//printf("%s rolls %d\n", p[i].a.name, p[i].k);
		p[i].same = 0;
	}

	for (int i=0; i < n; i++) printf("Player %d : %s\n", i+1,  p[i].a.name);
	printf("\nEach player begins with LKR %'d\n\n", cash);
	for (int i=0; i < n; i++) printf("%s rolls %d\n", p[i].a.name, p[i].k);

	//bubble sort
	int a=0, b=n, f, e;
resort:
	for (int i=a; i < b; i++){
		for (int j=a; j < (b-1); j++){
			if(p[j].k < p[j+1].k){
				tmp = p[j];
				p[j] = p[j+1];
				p[j+1] = tmp;
			}
			if(p[j].k == p[j+1].k){	
				p[j].same = 255;
				p[j+1].same = 255;
			}
		}
	}
	f = b; e = a;
	for (int i=a; i < b; i++){
		//puts(p[i].a.name);
		if (p[i].same){
			p[i].k = dice(&d);
			p[i].same = 0;
			printf("%s re-rolls %d\n", p[i].a.name, p[i].k);
			if (i < f) f = i;
			if (i > e) e = i;
		}
	}
	if ((f!=b)||(e!=a)){ a = f; b = e; goto resort; }
	puts("");
	printf("%s will begin the game\n", p[0].a.name);
	puts("\nTurn order:");
	for (int i=0; i < n; i++){
		printf("%s\n", p[i].a.name);
		*(plyrs+i) = p[i].a;
	}
}

