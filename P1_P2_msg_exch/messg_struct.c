#include "messg_struct.h"
#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <string.h>

//the struct of the message that we exchange

void messg_init(messg **mymessage)
{
    messg *message;
    message = malloc(sizeof(messg));
    message->p1_p2=1;
    message->retransmission=0;
    *mymessage=message;

    
}

void delmessg(messg *message)
{

    free(message);
}
