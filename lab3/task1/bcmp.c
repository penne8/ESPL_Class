#include <stdio.h>
#include <stdlib.h>

typedef struct diff {
    long offset; /* offset of the difference in file starting from zero*/
    unsigned char orig_value;     /* value of the byte in ORIG */
    unsigned char new_value;     /* value of the byte in NEW */
} diff;

typedef struct node node;
 
struct node {
    diff *diff_data; /* pointer to a struct containing the offset and the value of the bytes in each of the files*/
    node *next;
};

void list_print(node *diff_list,FILE* output); 
     /* Print the nodes in diff_list in the following format: byte POSITION ORIG_VALUE NEW_VALUE. 
        Each item followed by a newline character. */

node* list_append(node* diff_list, diff* data){
     /* Add a new node with the given data to the list,
        and return a pointer to the list (i.e., the first node in the list).
        If the list is null - create a new entry and return a pointer to the entry.*/
    struct node *newNode = malloc(sizeof(struct  node));
    newNode->diff_data = data;
    newNode->next = NULL;

    if(diff_list == NULL)
        diff_list = newNode;
    else{
        struct node *curr = diff_list;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = newNode;
        newNode = diff_list;
    }

    return diff_list;
}

void list_free(node *diff_list); /* Free the memory allocated by and for the list. */

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