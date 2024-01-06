#include <stdio.h>
#include <ctype.h>
#include <string.h>

void stripString(char *str) {
    // Remove leading whitespace
    while (isspace((unsigned char)(*str))) {
        str++;
    }

    // Remove trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)(*end))) {
        end--;
    }

    // Null-terminate the trimmed string
    *(end + 1) = '\0';
}

int main() {
    char myString[] = "   This is a string with leading and trailing spaces.   ";
    printf("Before: \"%s\"\n", myString);

    stripString(myString);

    printf("After: \"%s\"\n", myString);

    return 0;
}