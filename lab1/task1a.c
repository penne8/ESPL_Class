#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void task1a(FILE *input){
	char c;
	while((c=fgetc(input)) != EOF){
		if(c != '\n')
			printf("%d ", (int)c);
		else
			printf("\n");
	}
	printf("\n");
}

int main (int argc, char **argv){
	FILE *input = stdin;
	
	task1a(input);

	printf("exiting...\n");

	return 0;
}