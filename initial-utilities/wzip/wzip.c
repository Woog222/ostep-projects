#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    } 


    char current_char = EOF, temp;
    int cnt = 0;

    for (int i=1; i < argc; ++i) {
        FILE *fp= fopen(argv[i], "r");
        if (fp == NULL) {
            printf("cannot open file\n");
            exit(1);
        }

        while( (temp = fgetc(fp)) != EOF ) {
            if (current_char == EOF) {
                cnt =1;
                current_char = temp;
            }
            else if (temp == current_char) {
                cnt++;
            }
            else {
                assert (1 == fwrite(&cnt, sizeof(int), 1, stdout ));
                assert (1 == fwrite(&current_char, sizeof(char), 1, stdout));
                cnt = 1;
                current_char = temp;
            }
        }

        fclose(fp);
    }

    // the remaining one
    assert (1 == fwrite(&cnt, sizeof(int), 1, stdout ));
    assert (1 == fwrite(&current_char, sizeof(char), 1, stdout));
    return 0;
}