#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hexa_dec.h"
#include "filereader.h"

//This function has multiple purposes

int read_p(FILE *trace,int q,char* method,int *num_of_reads,int *num_of_writes,int *end_flag,bucket_node **buckets,int *avail_mem,int *pos,bucket_node **buckets_other,bucket_node *combined,int *pg_fault)
{
    char ch;
    char r_w;
    char *hexa_num;
    int i=0;
    int flag=1;
    long int k;
    int bucket_num;
    int min;
    int min_pos;
    hexa_num=malloc(sizeof(10));
    while((k=(ch=fgetc(trace))) != EOF && i<q) //Reading the file
    {
        if (ch==' ')
        {
            //printf("%s ",hexa_num);
            flag=0;

        }
        if(ch=='\n') //end of line
        {
            strcpy(hexa_num,"");
            flag=1;
            i++;
            *pos=*pos+1;
            if(i>=q)
            {
                break;
            }
        }
        if(flag && ch!='\n')    
        {    
            strncat(hexa_num,&ch,1);
        }
        if(!flag && ch!=' ')
        {
            if(ch=='R')
            {
                r_w=0;
            }
            else
            {
                r_w=1;
            }
            //printf("%c ",ch);
            k=hexa_to_dec(hexa_num); //hexadecimal to decimal
            bucket_num=hash_funct(k);   //hashing
            if((bucket_find(buckets[bucket_num],k)) && bucket_find(buckets_other[bucket_num],k)) //both hashes have the element
            {
                bucket_update(buckets[bucket_num],k,*pos,r_w,1);
                bucket_update(combined,k,*pos,r_w,1); //This is where the replacement algorithm will take place
                bucket_update(buckets_other[bucket_num],k,*pos,r_w,1);
            }
            else if (bucket_find(buckets[bucket_num],k)) //current table
            {
                bucket_update(buckets[bucket_num],k,*pos,r_w,1);
                bucket_update(combined,k,*pos,r_w,1);
            }
            else if (bucket_find(buckets_other[bucket_num],k)) //other table
            {
                bucket_update(combined,k,*pos,r_w,1);
                bucket_update(buckets_other[bucket_num],k,*pos,r_w,1);
                bucket_update(buckets[bucket_num],k,*pos,r_w,1);
            }
            else
            {
                if (*avail_mem>0)
                {//if we still have available memory
                    *avail_mem=*avail_mem-1;                    
                    bucket_add(buckets[bucket_num],k,*pos,r_w,0);
                    bucket_add(combined,k,*pos,r_w,0);
                    *pg_fault=*pg_fault+1;
                }
                else
                {// else
                    if(strcmp(method,"LRU")==0)
                    {
                        min=bucket_min(combined);   //find the least recently used
                    }
                    else
                    {
                        min=sca_bucket_min(combined); //likewise but with sca
                    }
                    if(find_r_w(combined,min))  //if it was a W
                    {
                        *num_of_writes=*num_of_writes+1;
                    }
                    
                    list_del_item(combined,min); //delete it to free space
                    min_pos=hash_funct(min);    //delete it from the hash table
                    
                    
                    if (bucket_find(buckets[min_pos],min))
                    {
                        list_del_item(buckets[min_pos],min);
                    }
                    if(bucket_find(buckets_other[min_pos],min))
                    {
                        list_del_item(buckets_other[min_pos],min);
                    }
                    bucket_add(buckets[bucket_num],k,*pos,r_w,0);   //add the new one
                    bucket_add(combined,k,*pos,r_w,0);
                    *pg_fault=*pg_fault+1;  // page fault counter
                    
                }
                    
            }
        }
    }

    if(k==EOF) //we have finally read the whole file
    {
        printf("end of file \n");
        *end_flag=0;
    }
   // printf("this is i though %d",i);
    return i;   //line counter


}