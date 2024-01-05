#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int areFilesSame(const char* file1, const char* file2) {
    struct stat stat1, stat2;

    if (stat(file1, &stat1) == -1) {
        perror("Error getting information for file1");
        return -1;
    }

    if (stat(file2, &stat2) == -1) {
        perror("Error getting information for file2");
        return -1;
    }

    return (stat1.st_ino == stat2.st_ino);
}

void* malloc_wrapper(size_t size) {
    void* ret = malloc(size);
    if (ret == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    return ret;
}

/*
    simple stack using linked list
*/
typedef struct node__{
    char* text;
    struct node__* next;
} node;

node* init_stack() {
    node* head = (node*) malloc_wrapper(sizeof(node));
    head->next = head;
    return head;
}

void push(node* head, char* text) {
    node* new_node = (node*) malloc_wrapper(sizeof(node));
    new_node->next = head->next;
    new_node->text = text;
    head->next = new_node;
    return;
}

node* pop(node* head) {
    node* ret = head->next;
    if (ret == head) { // empty
        return NULL;
    }

    head->next = ret->next;
    return ret;
}

int main(int argc, char* argv[]) {

/*
0. Input and Output Configuration

The program follows a simple rule: read from stdin and write to stdout.

To implement this, substitute the appropriate input and output streams for stdin and stdout.
*/
    // Invalid case
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // Read from an input file, replace stdin with an input file stream.
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    // Output file specified, replace stdout with an output file stream.
    if (argc == 3) {
        if (freopen(argv[2], "w", stdout) == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            exit(1);
        }

        // same file?
        if (areFilesSame(argv[1], argv[2])) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
    }

/*
1. Do the task using stack.
*/

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    node* head = init_stack();

    while ((read = getline(&line, &len, stdin)) != -1) {
        // line has dynamically allocated memory.
        push(head, line);
        line = NULL; len = 0;
    }

    node* temp;
    while ( (temp = pop(head)) != NULL) { // till it's empty
        printf("%s", temp->text);
        free(temp->text); free(temp);
    }

    return 0;
}
