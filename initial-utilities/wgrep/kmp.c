#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* kmp_check(char* text, char* pattern) {

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


    // count
    int cnt= 0;
    int* ret = (int*) malloc( (n+1) * sizeof(int));
    memset(ret, -1, (n+1) * sizeof(int));
    i =0; j=0;
    for (i=0; i<n; ++i) {
        while (j>0 && text[i] != pattern[j]) {
            j = fail_array[j-1];
        }

        if ( text[i] == pattern[j]) {
            j++;
            if (j==m) { // find!
                ret[cnt++] = i-m+2;
                j = fail_array[j-1];
            }
        }
    }

    free(fail_array);
    return ret;
}

int main() {
    char pattern[2000000], text[2000000];
    gets(text);
    gets(pattern);

    // printf("%s\n%s\n", pattern, text);

    int *ret = kmp_check(text, pattern);
    int cnt =0;
    for (int i=0; i<5000000; ++i) {
        if (ret[i] != -1) {
            cnt ++;
        }
        else {
            break;
        }
    }
    printf("%d\n", cnt);
    for (int i=0; i<cnt; ++i) {
        printf("%d\n", ret[i]);
    }


    free(ret);
    return 0;
}