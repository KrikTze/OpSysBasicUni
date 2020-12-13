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

#define PARENT 0 //parent process
#define CHILD 1 // child process
#define CREATION_ERROR -1 //error_finding
#define DELETION_ERROR -2
#define NSEMS 2

int main(int argc,char * argv[]) //Input the chance to change each character (int)
{//MAIN
    int op[2];
    op[0]=0;
    op[1]=0;
    //initialize shared memory and semaphores
    key_t shmkeyp1_enc1;
    if((shmkeyp1_enc1=ftok(FILENAME,1)) == -1)
    {
        printf("Couldnt create key \n");
        return CREATION_ERROR;
    }
    key_t semkeyp1_enc1;
    if((semkeyp1_enc1=ftok(FILENAME,2))==-1)
    {
        printf("Couldnt create key\n");
        return CREATION_ERROR;
    }
    int  semp1_enc1_id;
    if((semp1_enc1_id=sem_create(semkeyp1_enc1,NSEMS,op))<0)
    {
        printf("Semaphore creation error");
        return CREATION_ERROR;
    }
    messg *block_ptr;
    if((block_ptr=attach_mem(FILENAME,BLOCK_SIZE,shmkeyp1_enc1))==NULL)
    {
        printf("Couldnt create shm");
        return CREATION_ERROR;
    }
    
    int enc1_pid;
    enc1_pid=fork(); // forking to create enc1
    if(enc1_pid==-1)
    {
        printf("Creation Error");
        return -1;
    }

    if (enc1_pid==0)
    {   //ENC1
        int channel_pid;
        //initialize  shared memory and semaphores
        key_t shmkeyenc1_chan;
        if((shmkeyenc1_chan=ftok(FILENAME,3)) == -1)
        {
        printf("Couldnt create key \n");
        return CREATION_ERROR;
        }

        key_t semkeyenc1_chan;    
        if((semkeyenc1_chan=ftok(FILENAME,4)) == -1)
        {
        printf("Couldnt create key \n");
        return CREATION_ERROR;
        }
        messg *block_ptr_enc1_chan;
        int sem_enc1_chan_id;
        if ((sem_enc1_chan_id=sem_create(semkeyenc1_chan,NSEMS,op))<0)
        {
            printf("Couldnt create semaphores \n");
            return CREATION_ERROR;
        }
        if ((block_ptr_enc1_chan=attach_mem(FILENAME,BLOCK_SIZE,shmkeyenc1_chan))==NULL)
        {
            printf("Couldnt share memory\n");
            return CREATION_ERROR;
        }
        channel_pid=fork(); // we use fork again in enc1 to create the channel of communication between the two processes
        if(channel_pid==0)
        {   //CHANNEL
            //initialize
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
            if ((sem_chan_enc2_id=sem_create(semkeychan_enc2,NSEMS,op))<0)
            {
                printf("Couldnt create semaphores \n");
                return CREATION_ERROR;
            }
            if ((block_ptr_chan_enc2=attach_mem(FILENAME,BLOCK_SIZE,shmkeychan_enc2))==NULL)
            {
                printf("Couldnt share memory\n");
                return CREATION_ERROR;
            }
            block_ptr_chan_enc2->retransmission=0;

            char *channel_printer;
            int chance;
            chance = atoi(argv[1]); // the given probability
            messg *mymessage_chan;
            messg_init(&mymessage_chan);
            printf("This is the probability for each individual character: %d \n",chance);
            channel_printer = malloc(100);
            while(1)
            {
                while(1)
                {//we always assume that p1 sends the first message
                // then its p2 turn to send
                // so this back&forth can help us determine channels job each time
                //p1->p2
                    sem_down(sem_enc1_chan_id,CHILD);
                    if (block_ptr_enc1_chan->retransmission==1)
                    {
                       //sem_up(semp1_enc1_id,PARENT);
                       break;
                    }
                    
                    memcpy(channel_printer,block_ptr_enc1_chan->message,100);
                    //printf("This is the message from Channel %s\n",channel_printer);
                    change(channel_printer,chance);
                    memcpy(block_ptr_chan_enc2->message,channel_printer,100);
                    memcpy(block_ptr_chan_enc2->hash,block_ptr_enc1_chan->hash,MD5_DIGEST_LENGTH);
                    sem_up(sem_chan_enc2_id,CHILD); 

                    sem_down(sem_chan_enc2_id,PARENT);

                    block_ptr_enc1_chan->retransmission=block_ptr_chan_enc2->retransmission;
                    sem_up(sem_enc1_chan_id,PARENT);

                }
                //TERM is what we use to terminate the processes
                if (memcmp(channel_printer,"TERM",4)==0)
                {
                    sem_up(sem_enc1_chan_id,PARENT);
                    break;
                }

                while(1)
                {
                    //now its from p2->p1
                    sem_down(sem_chan_enc2_id,PARENT);
                    if (block_ptr_chan_enc2->retransmission==1)
                    {
                       break;
                    }
                    memcpy(channel_printer,block_ptr_chan_enc2->message,100);
                    change(channel_printer,chance);
                    memcpy(block_ptr_enc1_chan->message,channel_printer,100);
                    memcpy(block_ptr_enc1_chan->hash,block_ptr_chan_enc2->hash,MD5_DIGEST_LENGTH);

                    sem_up(sem_enc1_chan_id,PARENT);


                    sem_down(sem_enc1_chan_id,CHILD);
                    block_ptr_chan_enc2->retransmission=block_ptr_enc1_chan->retransmission;
                    //printf("This is the retransmission value %d \n",block_ptr_chan_enc2->retransmission);
                    sem_up(sem_chan_enc2_id,CHILD);//anoigw tis 6 pules

                }

                if (memcmp(block_ptr_enc1_chan->message,"TERM",4)==0)
                {
                    sem_up(sem_chan_enc2_id,CHILD);
                    break;
                }                  


            }
            detach_mem(block_ptr_chan_enc2);
            destroy_mem(FILENAME,shmkeychan_enc2);
            free(channel_printer);
            sem_remove(sem_chan_enc2_id);
            exit(0);
        }
        else
        {//ENC1
            int cmp;
            int transmit_attempts=0;
            messg *mymessage_enc1;
            messg_init(&mymessage_enc1);
            while(1)
            {

                sem_down(semp1_enc1_id,CHILD);
                mymessage_enc1->retransmission=0;
                block_ptr_enc1_chan->retransmission=0;

                while(1)
                {
                    //printf("everything is fine ?\n");
                    //We resend the message as many times as needed
                    memcpy(mymessage_enc1->message,block_ptr->message,100);
                    //printf("This one is from the enc1 but yallready know %s",mymessage_enc1->message);
                    memcpy(block_ptr_enc1_chan->message,mymessage_enc1->message,100); 
                    MD5(mymessage_enc1->message,strlen(mymessage_enc1->message),mymessage_enc1->hash);
                    memcpy(block_ptr_enc1_chan->hash,mymessage_enc1->hash,MD5_DIGEST_LENGTH);
                    mymessage_enc1->retransmission = block_ptr_enc1_chan->retransmission;
                    transmit_attempts++;
                    sem_up(sem_enc1_chan_id,CHILD);
                    if(mymessage_enc1->retransmission==1) //means that it got through
                    {

                        printf("%d Transmit attempts \n",transmit_attempts-1);
                        //sem_down(sem_enc1_chan_id,PARENT);
                        break;
                        
                    }
                    sem_down(sem_enc1_chan_id,PARENT);

                }
                transmit_attempts=0;
                if (memcmp(block_ptr_enc1_chan,"TERM",4)==0) //checking the message was TERM
                {
                    sem_up(semp1_enc1_id,PARENT);
                    //printf("So far so good \n");
                    //fflush(stdout);
                    break;
                }
                while(1)
                {
                    //We keep asking for the message if we determine that it's wrong
                    sem_down(sem_enc1_chan_id,PARENT);
                    mymessage_enc1->retransmission=0;
                    block_ptr_enc1_chan->retransmission=0;

                    MD5(block_ptr_enc1_chan->message,strlen(block_ptr_enc1_chan->message),mymessage_enc1->hash);
                    cmp=memcmp(mymessage_enc1->hash,block_ptr_enc1_chan->hash,MD5_DIGEST_LENGTH);
                    if(cmp!=0)
                    {
                        printf("We are going to need a retransmission\n");
                        sem_up(sem_enc1_chan_id,CHILD);
                    }
                    else
                    {
                        block_ptr_enc1_chan->retransmission=1;
                        sem_up(sem_enc1_chan_id,CHILD);
                        break;
                    }                    
                }


                memcpy(block_ptr->message,block_ptr_enc1_chan->message,100);
                sem_up(semp1_enc1_id,PARENT);

                if (memcmp(block_ptr->message,"TERM",4)==0)
                {
                    break;
                }            
            }
            int grandchil_pid;
            wait(&grandchil_pid);
            delmessg(mymessage_enc1);
            detach_mem(block_ptr_enc1_chan);
            destroy_mem(FILENAME,shmkeyenc1_chan);
            sem_remove(sem_enc1_chan_id);
            exit(0);
        }
    }
    else
    {//P1



        char buf[100];
        char *p1_printer;
        p1_printer=malloc(100);
        
        while(1)
        {
            //Sending the message
            printf("This is p1, please enter your message\n");            
            fgets(buf,100,stdin);
            memcpy(block_ptr->message,buf,100);
            sem_up(semp1_enc1_id,CHILD);
            
            if (memcmp(buf,"TERM",4)==0)
            {
                break;
            }
            //Receiving the message
            sem_down(semp1_enc1_id,PARENT);
            memcpy(p1_printer,block_ptr->message,100);
            block_ptr->p1_p2=1;
            printf("The message given by p2 was : %s \n",p1_printer);

            if (memcmp(p1_printer,"TERM",4)==0)
            {
                break;
            }            
        }
        int childpid;//freeing and stuff
        wait(&childpid);
        free(p1_printer);
        detach_mem(block_ptr);
        destroy_mem(FILENAME,shmkeyp1_enc1);
        sem_remove(semp1_enc1_id);
    }
    
    
}