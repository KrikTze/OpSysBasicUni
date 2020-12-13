#ifndef MESSG_H
#define MESSG_H
#include <openssl/md5.h>

//the struct of the message that we exchange

struct messg
{
    unsigned char message[100];
    unsigned char hash[MD5_DIGEST_LENGTH];

    int p1_p2;
    int retransmission;

}typedef messg;

void messg_init(messg **mymessage);
void delmessg(messg *message);


#endif