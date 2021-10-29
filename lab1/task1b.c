#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void task1a(char c){
	printf("%d ", (int)c);
}

void getBinary(int dec, int *binary){
	int i;
	for(i=7; i>=0; i--){
		binary[i] = dec&1;
		dec >>= 1;
	}
}

void task1b(char c){
    int binary[8];
    getBinary((int)c, binary);
    int i;
    for(i=0; i<8; i++)
        printf("%d",binary[i]);
    printf(" ");
}

int main (int argc, char **argv){
	FILE *input = stdin;

    /* find flag*/
	char task = 'a';
	int i;
	for (i=1; i<argc; i++){
		if(strcmp(argv[i], "-b") == 0)
			task = 'b';
	}

    char c;
    while((c=fgetc(input)) != EOF){
		if(c == '\n')
			printf("\n");
		else{
            /* call the right function based on flag */
            switch (task){
                case 'b':
                    task1b(c);
                    break;

                default:
                    task1a(c);
            }
        }
	}

	printf("\nexiting...\n");

	return 0;
}