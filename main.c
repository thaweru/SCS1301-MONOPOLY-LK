#include "types.h"
#include <stdio.h>

int main(int argc, char *argv[]){
	init_game(argc, argv);
	char duble;
	for (int i=0; i < 24; i++)
	printf("%i, duble = %i\n", dice(&duble), duble);
}
