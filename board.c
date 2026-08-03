#include "types.h"
#include <stdlib.h>
#include <time.h>

int sqr_type_enum(char *str){
	int type;
	return type;
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
		if (strcmp(typ, "START") == 0){
			bord.s[i].type = START;
			puts("test if read");
		}
		if (strcmp(typ, "PROPERTY") == 0){
			bord.s[i].type = PROPERTY;
			fscanf(fp, "%[^,],", typ);
			bord.s[i].buyPrice = strtol(typ, NULL, 10);
			fscanf(fp, "%[^,],", typ);
			bord.s[i].baseRent = strtol(typ, NULL, 10);
			puts("test if read");
		}
		puts(bord.s[i].name);
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
