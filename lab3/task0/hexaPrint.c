#include <stdio.h>
#include <stdlib.h>

void printHex(char *buffer, int length){
    printf("%X ",buffer[0]);
}

int main(int argc, char **argv) {
    FILE *input = fopen(argv[1],"rb");
    char buffer[2];
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
