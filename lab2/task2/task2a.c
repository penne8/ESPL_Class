#include <stdio.h> 
 
void echo_printer(char c){ /* task 0 from lab1 - prints c to the standard output */
	fprintf("%c ",c);
}

void ascii_printer(char c){ /* task 1a from lab1 - print the ASCII code of c to the standard output*/
	printf("%d ", (int)c);
}

void binary_printer(char c){ /* task 1b from lab1 – print the binary representation of c to the standard output */
    int binary[8];
    int i;
	for(i=7; i>=0; i--){
		binary[i] = c&1;
		c >>= 1;
	}
    for(i=0; i<8; i++)
        printf("%d",binary[i]);
}

void lower_to_upper_printer(char c){ /* task 1c from lab1 – print c to the standard output in upper case */
    if ((int)c >= (int)'a' && (int)c <= (int)'z')
		printf("%c ", (char) (c & ~32));
	else
		printf("%d ", (char) (c & ~32));
}
 
void string_printer(char* str, void (*func_ptr) (char)){
  /* TODO: Complete during task 2b */
}
 
void string_reader(char* s){
  /* TODO: Complete during task 2c*/
}
 
void bitwise_and(char* s){
  /* TODO: Complete during task2c */
}
 
int main(int argc, char **argv){
  /* TODO: Test your code */
}
