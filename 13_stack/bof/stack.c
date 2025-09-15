#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include "minilib.h"

int bof(char *str)
{
    char buffer[100];
    strcpy(buffer, str);

    return 1;
}

int main(int argc, char **argv)
{
    char str[400];
    FILE *badfile;

    badfile = fopen("badfile", "r");
    fread(str, sizeof(char), 300, badfile);   

    bof(str);

    printf("Returned Properly\n");
    return 1;
}


