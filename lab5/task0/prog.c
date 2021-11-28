#include <stdio.h>

extern int open(char* str);
extern int close(int file_descriptor);

int main(int argc, char **argv){
    
    if(argc != 2)
        printf("Please enter a file name\n");

    else{
        int file_descriptor = open(argv[1]);
        printf("%d\n" , file_descriptor);
        int y = close(file_descriptor);
        printf("%d\n" , y);
    }

    return 0;
}