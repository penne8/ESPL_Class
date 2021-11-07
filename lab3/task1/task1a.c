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
    
    int pos = 0;
    struct node *curr = diff_list;
    if(diff_list == NULL){
        fprintf(output, "empty List");
    }
    else do{
        fprintf(output,"byte %d %d %d\n", pos, curr->diff_data->orig_value, curr->diff_data->new_value);
        curr = curr->next;
    }while(curr != NULL);
    
}

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

void list_free(node *diff_list){
     /* Free the memory allocated by and for the list. */
    
    if(diff_list != NULL)
        list_free(diff_list->next);
    free(diff_list);

}

int main(int argc, char **argv) {
    node *diff_list = NULL;

    diff diff1 = {1,'a','b'}; 
    diff_list = list_append(diff_list, &diff1);

    diff diff2 = {2,'a','c'}; 
    diff_list = list_append(diff_list, &diff2);

    list_print(diff_list, stdout);

    list_free(diff_list);
    return 0;
}