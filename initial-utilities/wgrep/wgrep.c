#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

int kmp_check(char* text, char* pattern) {
    int n = strlen(text); // txt length
    int m = strlen(pattern); // pattern length;
    int* fail_array = (int *)malloc(sizeof(int) * m);
    memset(fail_array, 0, sizeof(int) * m);

    // fail array 
    int i=0, j=0;
    for (i=1; i<m; ++i) {
        while( j > 0 && pattern[i] != pattern[j] ){
            j = fail_array[j-1];
        }

        if (pattern[i] == pattern[j]) {fail_array[i] = ++j;}
        else {fail_array[i] = 0;}
    }


    // check
    i =0; j=0;
    for (i=0; i<n; ++i) {
        while (j>0 && text[i] != pattern[j]) {
            j = fail_array[j-1];
        }

        if ( text[i] == pattern[j]) {
            j++;
            if (j==m) { // find 
                free(fail_array);
                return 1; 
            } 
        }
    }

    free(fail_array);
    return 0;
}

int main(int argc, char* argv[]) {
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    // check param validity
    if (argc < 2) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }

    // from files specified
    char* searchterm = argv[1];
    for (int i=2; i < argc; ++i) {
        FILE *fp = fopen(argv[2], "r");
        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            exit(1);
        }

        // read line by line
        while ( (read= getline(&line, &len, fp)) != -1 ) {
            if (kmp_check(line, searchterm)) {
                printf("%s", line);
            }
        }
        fclose(fp);
    }

    // no file specified, from stdin
    if (argc ==2) {
        //printf("no file specified. read from STDIN\n");
        while ( (read= getline(&line, &len, stdin)) != -1 ) {
            if (kmp_check(line, searchterm)) {
                printf("%s", line);
            }
        }
    }
        

    free(line); // deallocate
    return 0;
}

