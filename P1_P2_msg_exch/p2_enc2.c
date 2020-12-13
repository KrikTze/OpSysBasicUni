#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shared_create.h"
#include <openssl/md5.h>
#include "sema.h"

#include "rand_change.h"
#include "messg_struct.h"

#define PARENT 0 //anaferetai sto parent_process 
#define CHILD 1 //anaferetai sto child process
#define CREATION_ERROR -1   //error_finding
#define DELETION_ERROR -2
#define NSEMS 2


int main(int argc,char * argv[])
{
    int enc2_pid;
    int op[2];
    op[0]=0;
    op[1]=0;
    //initialize
    key_t shmkeyp2_enc2;
    if((shmkeyp2_enc2=ftok(FILENAME,6)) == -1)
    {
    printf("Couldnt create key \n");
    return CREATION_ERROR;
    }

    key_t semkeyp2_enc2;    
    if((semkeyp2_enc2=ftok(FILENAME,7)) == -1)
    {
    printf("Couldnt create key \n");
    return CREATION_ERROR;
    }
    messg *block_ptr_p2_enc2;
    int sem_p2_enc2_id;
    if ((sem_p2_enc2_id=sem_create(semkeyp2_enc2,NSEMS,op))<0)
    {
        printf("Couldnt create semaphores \n");
        return CREATION_ERROR;
    }
    if ((block_ptr_p2_enc2=attach_mem(FILENAME,BLOCK_SIZE,shmkeyp2_enc2))==NULL)
    {
        printf("Couldnt share memory\n");
        return CREATION_ERROR;
    }
    block_ptr_p2_enc2->retransmission=0;

    enc2_pid=fork(); // forking to create enc2
    if(enc2_pid==0)
    {//enc2
        key_t shmkeychan_enc2;
        if((shmkeychan_enc2=ftok(FILENAME,4)) == -1)
        {
            printf("Couldnt create key \n");
            return CREATION_ERROR;
        }

        key_t semkeychan_enc2;    
        if((semkeychan_enc2=ftok(FILENAME,5)) == -1)
        {
            printf("Couldnt create key \n");
            return CREATION_ERROR;
        }

        messg *block_ptr_chan_enc2;
        int sem_chan_enc2_id;
        if ((sem_chan_enc2_id=semget(semkeychan_enc2,NSEMS,0))<0)
        {
            printf("Couldnt create semaphores \n");
            return CREATION_ERROR;
        }
        if ((block_ptr_chan_enc2=attach_mem(FILENAME,BLOCK_SIZE,shmkeychan_enc2))==NULL)
        {
            printf("Couldnt share memory\n");
            return CREATION_ERROR;
        }
        char *enc2_printer;
        enc2_printer = malloc(100);
        messg *mymessage_enc2;
        messg_init(&mymessage_enc2);
        int cmp;
        int transmit_attempts=0;

        while(1)
        { //p1 always sends the first message so we wait
            while(1)
            { //waiting for the correct message 
                sem_down(sem_chan_enc2_id,CHILD); 
                //printf("so far so good \n");
                mymessage_enc2->retransmission=0;
                block_ptr_chan_enc2->retransmission=0;
                memcpy(enc2_printer,block_ptr_chan_enc2->message,100);
                MD5(block_ptr_chan_enc2->message,strlen(block_ptr_chan_enc2->message),mymessage_enc2->hash);
                cmp=memcmp(mymessage_enc2->hash,block_ptr_chan_enc2->hash,MD5_DIGEST_LENGTH);
                //checking if it was corrupted
                if(cmp!=0)
                {
                    //we have to resend
                    printf("The message was corrupted \n");
                    sem_up(sem_chan_enc2_id,PARENT);

                }
                else
                {
                    //we notify enc1 that everying worked properly
                    block_ptr_chan_enc2->retransmission=1;
                    sem_up(sem_chan_enc2_id,PARENT);
                    break; // 
                }
                //printf("This is enc2, %s\n",enc2_printer);
            }
            memcpy(block_ptr_p2_enc2->message,enc2_printer,100);
            sem_up(sem_p2_enc2_id,PARENT);

            if (memcmp(enc2_printer,"TERM",4)==0)
            {
                break;
            }   

            sem_down(sem_p2_enc2_id,CHILD);
            mymessage_enc2->retransmission=0;
            block_ptr_chan_enc2->retransmission=0;
            while(1)
            { // now it's p2->p1
                memcpy(block_ptr_chan_enc2->message,block_ptr_p2_enc2->message,100);
                //printf("We here\n");
                MD5(block_ptr_p2_enc2->message,strlen(block_ptr_p2_enc2->message),mymessage_enc2->hash);

                memcpy(block_ptr_chan_enc2->hash,mymessage_enc2->hash,MD5_DIGEST_LENGTH);
                transmit_attempts++;                
                mymessage_enc2->retransmission=block_ptr_chan_enc2->retransmission;
                sem_up(sem_chan_enc2_id,PARENT); 
                if(mymessage_enc2->retransmission==1)
                {
                    //it finally got through
                    printf("%d Transmit Attempts \n",transmit_attempts-1);
                    break;
                }
                sem_down(sem_chan_enc2_id,CHILD);
            }
            transmit_attempts=0;
            mymessage_enc2->retransmission=0;    

            if (memcmp(block_ptr_p2_enc2->message,"TERM",4)==0)
            {
                break;
            }              
        }
        //free
        free(enc2_printer);
        delmessg(mymessage_enc2);
        detach_mem(block_ptr_chan_enc2);        
        detach_mem(block_ptr_p2_enc2);        

    }  
    else
    {//p2
        printf("This is p2, currently waiting for input \n");
        char *p2_printer;
        char buf[100];
        p2_printer=malloc(100);
        while(1)
        {//waiting for message
            sem_down(sem_p2_enc2_id,PARENT);
            memcpy(p2_printer,block_ptr_p2_enc2->message,100);
            printf("Finally : %s",p2_printer);
            if (memcmp(p2_printer,"TERM",4)==0)
            {
                break;
            }
        //sending message
            printf("Give me a message for p1\n");
            fgets(buf,100,stdin);
            memcpy(block_ptr_p2_enc2->message,buf,100);
            sem_up(sem_p2_enc2_id,CHILD);
            if (memcmp(buf,"TERM",4)==0)
            {
                break;
            }            
        }

        int child_pid;//free
        wait(&child_pid);
        detach_mem(block_ptr_p2_enc2);
        destroy_mem(FILENAME,shmkeyp2_enc2);
        sem_remove(sem_p2_enc2_id);
        free(p2_printer);
    }
    

}