#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    } 

    int cnt = 0;
    char c;

    for (int i=1; i < argc; ++i) {
        FILE *fp= fopen(argv[i], "r");
        if (fp == NULL) {
            printf("cannot open file\n");
            exit(1);
        }

        while (1) {
            if ( fread(&cnt, sizeof(int), 1, fp) < 1) {break;}
            c = fgetc(fp);
            while(cnt--) {printf("%c", c);}
        }
        fclose(fp);
    }

    return 0;
}




int p[2];
char *argv[2];


argv[0] = "wc";
argv[1] = 0;

pipe(p);
if (fork()==0) {
    //child process
    close(0);
    dup(p[0]);
    close(p[0]);
    close(p[1]);
    exec("/bin/wc", argv);
}
else {
    close(p[0]);
    write(p[1], "hello world\n", 12);
    close(p[1]);
}