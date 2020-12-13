#ifndef SEM_H
#define SEM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

//Simple functions for semaphores

union semun {                   /* Used in calls to semctl() */
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#define PATH "sema.c"

int sem_create(key_t key, int nsems, int op[2]); // Initialize semaphores

int sem_down(int semid, int sem_num); //Down the semaphore

int sem_up(int semid, int sem_num);  //Up the semaphore

int sem_remove(int semid); // Delete the semaphore


#endif