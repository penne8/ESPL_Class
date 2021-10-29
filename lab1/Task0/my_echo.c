#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
	FILE* output = stdout;
	int i;
	for(i=1; i<argc; i++){
		fprintf(output, "%s ",argv[i]);
	}
	fprintf(output, "\n");
	if (output != stdout)
		fclose(output);
	return 0;
}
