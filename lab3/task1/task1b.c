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

void list_print(node *diff_list,FILE* output){
     /* Print the nodes in diff_list in the following format: byte POSITION ORIG_VALUE NEW_VALUE. 
        Each item followed by a newline character. */

    struct node *curr = diff_list;
    if(diff_list == NULL){
        fprintf(output, "empty List");
    }
    else do{
        fprintf(output,"byte %ld %X %X\n", curr->diff_data->offset, curr->diff_data->orig_value, curr->diff_data->new_value);
        curr = curr->next;
    }while(curr != NULL);
    
}

node* list_append(node* diff_list, diff* data){
     /* Add a new node with the given data to the list,
        and return a pointer to the list (i.e., the first node in the list).
        If the list is null - create a new entry and return a pointer to the entry.*/
    
    if(diff_list == NULL){
        node *newNode = malloc(sizeof(node));
        newNode->diff_data = data;
        newNode->next = NULL;
        return newNode;
    }
    else{
        diff_list->next = list_append(diff_list->next, data);
        return diff_list;
    }
}

void list_free(node *diff_list){
     /* Free the memory allocated by and for the list. */
    
    if(diff_list != NULL){
        list_free(diff_list->next);
        free(diff_list->diff_data);
    }
    free(diff_list);

}

int main(int argc, char **argv) {
    FILE *origFile = fopen(argv[argc-2],"rb");
    fseek(origFile, 0L, SEEK_END);
    int origSize = ftell(origFile);
    rewind(origFile);

    FILE *newFile = fopen(argv[argc-1],"rb");
    fseek(newFile, 0L, SEEK_END);
    int newSize = ftell(newFile);
    rewind(newFile);

    int compareSize = (origSize < newSize) ? origSize : newSize;

    node *diff_list = NULL;
    int i;
    for(i=0; i<compareSize; i++){
        unsigned char origBuffer[1];
        unsigned char newBuffer[1];
        fread(origBuffer, sizeof(char), 1, origFile);
        fread(newBuffer, sizeof(char), 1, newFile);
        
        if( origBuffer[0] != newBuffer[0]){
            diff *currData = malloc(sizeof(diff));
            currData->offset = i;
            currData->orig_value = origBuffer[0];
            currData->new_value = newBuffer[0];
            diff_list = list_append(diff_list, currData);
        }
    }

    fclose(origFile);
    fclose(newFile);
     
    list_print(diff_list, stdout);

    list_free(diff_list);
    return 0;
}