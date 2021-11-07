#include <stdio.h>
#include <stdlib.h>

void printHex(char *buffer, int length){
    int i;
    for(i=0; i<length; i++)
        printf("%X ",buffer[i]);
}

int main(int argc, char **argv) {
    FILE *input = fopen(argv[1],"rb");
    char buffer[20];
    while(1){
        int len = fread(buffer, sizeof(char), 1, input);
        if(len > 0) {
            printHex(buffer, len);
        } else { /* EoF */
            break;
        }
    }
    
    printf("\n");
    return 0;
}
