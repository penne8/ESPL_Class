#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
 
void echo_printer(char c){ /* task 0 from lab1 - prints c to the standard output */
	printf("%c",c);
}

void ascii_printer(char c){ /* task 1a from lab1 - print the ASCII code of c to the standard output*/
	printf("%d", c);
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
  if (c >= 'a' && c <= 'z')
		printf("%c", c & ~32);
	else
		printf("%c", c & ~32);
}
 
void string_printer(char* str, void (*func_ptr) (char)){
  int i;
  for(i=0; i<strlen(str); i++){
    if(str[i] != '\n')
        func_ptr(str[i]);
    printf(" ");
  }
  printf("\n");
}
 
void string_reader(char* s){
	fgets(s, 10, stdin);
}

void binary_array(char c, int *binary){
  int i;
	for(i=7; i>=0; i--){
		binary[i] = c&1;
		c >>= 1;
	}
}

void bitwise_or(char* s){
  int res_bin[] = {0,0,0,0,0,0,0,0};

  int i;
  for(i=0; i<strlen(s); i++){
    int binary[8];
    binary_array(s[i], binary);

    int j;
    for(j=0; j<8; j++){
      res_bin[j] |= binary[j];
    }
  }
  
  char bin_string[9];
  int hex_num = 0;
  for(i=0; i<8; i++){
    bin_string[i] = res_bin[i] ? '1':'0';
    hex_num = (hex_num<<1)|res_bin[i];
  }
  bin_string[8] = '\0';

  printf("the result is %X in hex and %s in binary\n", hex_num, bin_string);

}

typedef struct fun_desc{
    char *name;
    void (*fun)(char);
} fun_desc;

int main(int argc, char **argv){
  fun_desc functions[] =    {{"echo printer",echo_printer},
                            {"ASCII printer",ascii_printer},
                            {"binary printer",binary_printer},
                            {"lower to upper",lower_to_upper_printer}};
  printf("Please enter a string (0<size<=10):\n");                
  
  char s[10];
  string_reader(s);

  printf("Please choose printer type:\n");  
  int i;
  for(i=0; i<4; i++){
      printf("%d) %s\n",i,functions[i].name);
  }
  printf("4) bitwise or\n\n");

  char c;
  printf("Option: ");
  while((c=fgetc(stdin)) != EOF){
    if(c=='\n')
        continue;
    char temp[] = {c,'\0'};
    int choosen = atoi(temp);
    printf("choosen: %d\n",choosen);
    if(choosen>=0 && choosen<=3)
        string_printer(s, functions[choosen].fun);
    else if(choosen == 4)
        bitwise_or(s);
    else
        printf("Please select 1-4 or CTRL-D to exit.\n");
    printf("Option: ");
  }
  printf("DONE.\n");

  return 0;
}
