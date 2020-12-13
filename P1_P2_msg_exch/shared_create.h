#ifndef SHARED_H
#define SHARED_H

#define BLOCK_SIZE 2048
#define FILENAME "p1_enc1_chan.c"

#include <stdbool.h>
#include "messg_struct.h"
//general functions for shared memory usage
messg * attach_mem(char *filename,int size,key_t key);
bool detach_mem(messg *block);
bool destroy_mem(char *filename,key_t key);



#endif