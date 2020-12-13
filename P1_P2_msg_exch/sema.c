#include "sema.h"
//Simple semaphore functions


int sem_create(key_t key, int nsems, int op[2])
{
    //currently it can only create a pair of semaphores
    int semid;
    union semun arg;

    if ((key < 0) || (nsems <= 0))
        return -1;

    semid = semget(key, nsems, 0666 | IPC_CREAT);

    if (semid < 0){
        
        return -1;
    }

    int  i=0, current_sem=0;
    
    while(i<2)
    {

        
        int result = semctl(semid, current_sem, SETVAL, op[i]);
        if (result < 0){
            printf("semctl() function error.\n\n");
            return -2;
        }
        i++;
        current_sem++;
    }

    return semid;

}

int sem_down(int semid,int sem_num)
{
    struct sembuf sb;

    if((semid < 0) || (sem_num < 0))
        return -1;

    sb.sem_num = sem_num;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    return semop(semid,&sb,1);
}


int sem_up(int semid, int sem_num)
{
    struct sembuf sb;

    if((semid < 0) || (sem_num < 0))
        return -1;

    sb.sem_num = sem_num;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    return semop(semid,&sb,1);
}


int sem_remove(int semid)
{

    if(semid < 0)
        return -1;

    return semctl(semid,0,IPC_RMID);
}