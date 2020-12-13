#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <time.h>
#include "rand_change.h"
#include "messg_struct.h"
time_t t;

//semi-generic randomizers

int ran_val(int n)
{
    int val;
    val = rand() % n;
    val++;
    return val;
}

int rand_char()
{
    int val;
    val = rand() % 65 +60;
    //printf("%d val\n",val);
    return val;
}

void change(char *messg,int chance)
{//changes the message
    int r_val;
    char *k;
    int i;
    k=malloc(1);
    for(i=0;i<strlen(messg);i++)
    {
        r_val = ran_val(100);
        if(r_val<=chance)
        {
            *k=rand_char();

            if((memcmp(&messg[i],k,1))==0)
            {
                //it picks a random ascii character, if its the same it just adds 1
                printf("This is quite rare rand_char == current_char\n");
                messg[i]=messg[i]+1;
            }
            else
            {
                memcpy(&messg[i],k,1);
            }
        }
    }
    free(k);
}



