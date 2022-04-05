#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "filereader.h"
#include "hash.h"

#define PAGE_SIZE 4096

int main(int argc , char *argv[])
{

    char *method;
    int q;
    int avail_mem;
    int max_avail_mem;
    int method1;
    int method2;
    int max_per_f=INT_MAX;
    int num_of_reads, num_of_writes =0; 
    if (argc!=4 && argc!=5)     //Arguements
    {
       printf("Wrong arguements \n");
       return -1;
    }
    else
    {
        method=malloc(sizeof(3));
        avail_mem=atoi(argv[2]);
        q=atoi(argv[3]);
        strcpy(method,argv[1]);
        if(q>avail_mem)
        {
            printf("Q is larger than Avail memory \n");
           // return -2;
        }
        method1=strcmp(method,"LRU");
        method2=strcmp(method,"SCA");
        if (method1==0 && method2==0)
        {
            printf("Invalid method of replacement");
            return -2;
        }
        if(argc==5)
        {
            max_per_f=atoi(argv[4]);
        }
    }

    FILE *trace1;
    FILE *trace2;
    trace1=fopen("bzip.trace","r"); //opening the files
    trace2=fopen("gcc.trace","r");
    int counter=0;
    int pos=1;
    int end_flag1=1;
    int end_flag2=1;
    int pg_fault=0;
    max_avail_mem=avail_mem;
    bucket_node **buckets1;
    bucket_node **buckets2;
    bucket_node *combined;
    buckets1=malloc(NUM_BUCKETS* sizeof(bucket_node*)); //allocating space for the 2 hash tables
    buckets2=malloc(NUM_BUCKETS* sizeof(bucket_node*));
    combined = malloc(sizeof(bucket_node)); //allocating space for LRU or SCA
    int j;
    bucket_init(&combined);
    for(j=0;j<NUM_BUCKETS;j++)  //Init  
    {
        bucket_init(&buckets1[j]);
        bucket_init(&buckets2[j]);
    }
    printf("%s \n",method);

    while(end_flag1==1 && end_flag2==1 && counter <max_per_f) //Reading the files
    {
    counter+=read_p(trace1,q,method,&num_of_reads,&num_of_writes,&end_flag1,buckets1,&avail_mem,&pos,buckets2,combined,&pg_fault);
   // printf("\nSwitching things up \n \n");
    counter+=read_p(trace2,q,method,&num_of_reads,&num_of_writes,&end_flag2,buckets2,&avail_mem,&pos,buckets1,combined,&pg_fault);

   // printf(" The counter %d \n",counter);
    }
    //    read_p(trace2,q,method,&num_of_reads,&num_of_writes);
        
    //}
    fclose(trace1);
    fclose(trace2);
    free(method);

    for(j=0;j<NUM_BUCKETS;j++) //Deletes
    {
        //printf("Bucket num = %d \n",j);
        //list_print(buckets1[j]);

        free_list(buckets1[j]);
        //printf("Bucket num = %d \n",j);
       // list_print(buckets2[j]);

        free_list(buckets2[j]);

    }
    printf("%d available memory\n",max_avail_mem); //Stats
    printf("Number of lines read %d \n",counter);

    free(buckets1);
    free(buckets2);
    free_list(combined);
    printf("%d Page faults \n",pg_fault);
    printf("Max per file %d \n",max_per_f);
    printf("Num of Writes %d \n",num_of_writes);
    return 0;
}