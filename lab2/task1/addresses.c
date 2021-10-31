#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5;
int addr6;

int foo();
void point_at(void *p);

int main (int argc, char** argv){
    int addr2;
    int addr3;
    char* yos="ree";
    char yos2[3];
    yos2[0] = 'r';
    yos2[1] = 'e';
    yos2[2] = 'e';

    int * addr4 = (int*)(malloc(50));
    printf("- &addr2: %p\n",&addr2);
    printf("- &addr3: %p\n",&addr3);
    printf("- foo: %p\n",foo);
    printf("- &addr5: %p\n",&addr5);
    
	point_at(&addr5);
	
    printf("- &addr6: %p\n",&addr6);
    printf("- yos: %p\n",yos);
    printf("- yos2: %p\n",yos2);
    printf("- addr4: %p\n",addr4);
    printf("- &addr4: %p\n",&addr4);
    return 0;
}

int foo(){
    return -1;
}

void point_at(void *p){
    int local;
	static int addr0 = 2;
    static int addr1;


    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;
    long dist4 = (size_t)&addr0 - (size_t)p;
    long dist5 = (size_t)&addr1 - (size_t)p;
    
    printf("dist1: (size_t)&addr6 - (size_t)p: %ld\n",dist1);
    printf("dist2: (size_t)&local - (size_t)p: %ld\n",dist2);
    printf("dist3: (size_t)&foo - (size_t)p:  %ld\n",dist3);
	printf("dist4: (size_t)&addr0 - (size_t)p:  %ld\n",dist4);
    printf("dist5: (size_t)&addr1 - (size_t)p:  %ld\n",dist5);

	printf("- addr0: %p\n", & addr0);
    printf("- addr1: %p\n",&addr1);
}

