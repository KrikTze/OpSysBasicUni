#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hexa_dec.h"
//converts a hexadecimal string to a decimal int

int hexa_to_dec(char *page)
{
    long int num;
    char *offset;
    offset=malloc(sizeof(5));
    memcpy(offset,page,5);
    num=strtol(offset,NULL,16);
    //printf("%ld \n",num);
    free(offset);
    return num;
}