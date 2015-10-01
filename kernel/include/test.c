#include "ctype.h"
#include <stdio.h>

struct test{
    int a;
    int b;
    char c;
};

int main(int argc, char *argv[])
{
    struct test t;
    int c =  (int)(unsigned char)'a';
    printf("isalnum: %c\n", c);

    return 0;
}
