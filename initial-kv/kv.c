#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FNAME "database.txt"
#define DELIM ","

void* malloc_wrapper(size_t size) {
    void* ret = malloc(size);
    if (ret == NULL){ 
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    return ret;
}

typedef struct item_{
    int key;
    char* value;
    struct item_* next;
} item;

item* head;

void clear() {
    item* cur = head->next;
    while(cur != head) {
        free(cur->value);
        head->next = cur->next;
        item* temp = cur; 
        cur = cur->next;
        free(temp);
    }
}

void init() {
    head = (item*) malloc_wrapper(sizeof(item));

    head->next = head;
}

void put(int key, char* value) {
    item* new_item = (item*) malloc_wrapper(sizeof(item));
    new_item->key = key; new_item->value= value;
    new_item->next = head->next;
    head->next = new_item;
}

item* get(int key) {
    item* cur = head->next;
    while(cur != head) {
        if (cur->key == key) return cur;
        cur = cur->next;
    }
    return NULL;
}

int delete(int key) {
    // return 1 for success, 0 no key found
    item* cur = head->next;
    item* prev = head;
    while(cur != head) {
        if (cur->key == key) {
            prev->next = cur->next;
            free(cur->value);
            free(cur);
            return 1;
        }
        prev = cur; cur = cur->next;
    }
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc == 1) {return 0;}

    /*
    load database
    */

    init();

    FILE* fp = fopen(FNAME, "r");
    if (fp != NULL) { // exist, load it
        char* line = NULL;
        size_t len=0;
        ssize_t read;

        while ( (read=getline(&line, &len, fp)) != -1) {
            if (line[read-1] == '\n') { // delete newline char
                line[read-1] = '\0';
            } 
            int key = atoi(strtok(line, DELIM));

            char* temp_value = strtok(NULL, DELIM);
            char* value = malloc_wrapper(strlen(temp_value)+1);
            strcpy(value, temp_value);

            put(key, value);
            
            free(line);
            line = NULL, len = 0;
        }
        fclose(fp);
    }

    fp = fopen(FNAME, "w");
    if (fp == NULL) {
        fprintf(stderr, "kv: cannot open %s", FNAME);
        exit(1);
    }

    /*
    MAIN TASK
    */

    char* token;
    for (int i=1; i<argc; ++i) {

        // command
        token = strtok(argv[i], DELIM);
        if (strcmp(token, "p") == 0) { // put
            
            int key = atoi(strtok(NULL, DELIM));
            char* temp_value = strtok(NULL, DELIM);
            char* value = malloc_wrapper(strlen(temp_value)+1);
            strcpy(value, temp_value);
            put(key, value);
        }
        else if (strcmp(token, "g")==0) { // get
            int key = atoi(strtok(NULL, DELIM));
            item* search_item = get(key);
            if (search_item == NULL) {
                printf("%d not found\n", key);
            }
            else {
                printf("%d,%s\n", search_item->key, search_item->value);
            }
        }
        else if (strcmp(token, "d")==0) { // delete
            int key = atoi(strtok(NULL, DELIM));
            if (!delete(key)) {
                printf("%d not found\n", key);
            }
        }
        else if (strcmp(token, "c")==0) { // clear
            clear();           
        }
        else if (strcmp(token, "a")==0) { //all
            item* cur = head->next;
            while (cur != head){
                printf("%d,%s", cur->key, cur->value);
            }
        }
        else { // bad command
            printf("bad command\n");
        }
    }
    
    // rewrite to the file

    item* cur = head->next;
    while(cur != head) {
        fprintf(fp, "%d,%s\n",cur->key, cur->value);
        cur = cur->next;
    }
    fclose(fp);
    clear();
    return 0;
}