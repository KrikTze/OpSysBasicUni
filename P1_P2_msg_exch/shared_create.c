#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "messg_struct.h"

#include "shared_create.h"
//general functions for shared memeory usage

static int get_shared_b(char * filename,int size,key_t key)
{

    if(key==-1)
    {
        return -1;
    }
    return shmget(key,size,0666 | IPC_CREAT);
}


messg * attach_mem(char *filename,int size,key_t key)
{
    int shm_bid = get_shared_b(filename,size,key);
    if (shm_bid==-1)
    {
        return NULL;
    }
    messg *blockptr;
    //messg_init(&blockptr);
    blockptr = shmat(shm_bid,NULL,0);
    if(blockptr == (messg *)-1)
    {
        return NULL;
    }
    return blockptr;
}

bool detach_mem(messg *blockptr)
{
    return (shmdt(blockptr)!=-1);
}

bool destroy_mem(char *filename,key_t key)
{
    int shm_bid = get_shared_b(filename,0,key);
    return (shmctl(shm_bid,IPC_RMID,NULL)!=-1);
}