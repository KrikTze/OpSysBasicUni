#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "hash.h"



int hash_funct(int num) //hast function 
{
    int bucket_num;
    bucket_num=num*3;
    bucket_num= bucket_num/2;
    bucket_num=bucket_num%NUM_BUCKETS;
    //printf("Bucket num %d ",bucket_num);

    return bucket_num;
}



void bucket_init(bucket_node **bucket)  //init
{
    bucket_node *temp;
    temp = malloc(sizeof(bucket_node));
    temp->next=NULL;
    temp->prev=NULL;
    temp->data.pos=INT_MAX;
    temp->data.pg_num=-10;
    temp->data.pos=INT_MAX;
    *bucket = temp;
}

int isEmpty(bucket_node *b) //checking if bucket is empty
{
    if (b->next==NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void bucket_add(bucket_node *b,int pg_num,int pos,int r_W,int sca)  //adds node  
{
    bucket_node *temp;
    temp = malloc(sizeof(bucket_node));
    temp->data.pos=pos;
    temp->data.pg_num=pg_num;
    temp->data.r_w=r_W;
    temp->data.sca=sca;
    bucket_node *trans = b;

    if(isEmpty(b))
    {
        //printf("The list was empty\n");
        b->next=temp;
        b->prev=temp;
        temp->next=b;
        temp->prev=b;
    }
    else
    {
        //printf("The list already has other elements\n");
        while (trans->next->data.pg_num!=-10)
        {
            trans = trans->next;
        }
        //printf("%s \n",trans->mydata.name); // shmainei oti twra eimai sto telos (vevaia einai kuklikoi kai doubly linked )
        // prepei na enhmerwsw to epomeno aftonou
        // to epomeno kai to prohgoumeno tou neou komvou
        // to prohgoumeno tou head
        trans->next=temp;
        temp->prev = trans;
        temp->next = b;
        b->prev=temp;

        
    }
    
}

void list_print(bucket_node *b) //prints bucket
{
    bucket_node *temp = b;
    if(isEmpty(b)==0)
    {    while(temp->next->data.pg_num!=-10)
        {
            printf("Pos: %d \nPg Number: %d \n",temp->next->data.pos,temp->next->data.pg_num); //works just fine + cyrcles through next and prev
            temp=temp->next;
        }
    }
}

int items_in_list(bucket_node *b)   //number of items in bucket
{
    bucket_node *temp = b;
    int counter = 0;
    if(isEmpty(b))
    {
        return 0;
    }
    else
    {
         while(temp->next->data.pg_num!=-10)
        {
            counter++;
            temp=temp->next;
        }
     //   printf("%d \n",counter);
        return counter;
    }
    
}

int bucket_find(bucket_node *b,int pg_num)  //check if something exists
{
    bucket_node *temp=b;
    if(isEmpty(b) == 0)
    { 
        while(temp->data.pg_num!=pg_num && temp->next->data.pg_num!=-10) //oso den einai afto pou zhtame kai den exoume kanei kuklo
        {
            temp=temp->next;
        }
        if(temp->data.pg_num==pg_num)
        {
            return 1;
        }    
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
void bucket_update(bucket_node *b,int pg_num,int pos,int r_w,int sca)   //updates a value
{
    int k;
    k=find_r_w(b,pg_num);
    list_del_item(b,pg_num);
    if(k==1 || r_w==1)
    {    
        bucket_add(b,pg_num,pos,1,sca);
    }
    else
    {
        bucket_add(b,pg_num,pos,0,sca);
    }
    
}

void list_del_item(bucket_node *b,int pg_num)   //deletes bucket node
{
    bucket_node *temp=b;
    int no_items = 0 ;
    if(isEmpty(b) == 0)
    { 
        while(temp->data.pg_num!=pg_num && temp->next->data.pg_num!=-10) //oso den einai afto pou zhtame kai den exoume kanei kuklo
        {
            temp=temp->next;
        }
        if(temp->data.pg_num==pg_num) // node detected
        //prepei to epomeno tou prohgoumenou na ginei to diko tou epomeno
        // prepei to prohgoumeno tou epomenou na ginei to diko tou prohgoumeno
        //prepei na kanw free ton komvo
        //akraies periptwseis a) mono to head, to afhnw, b) head kai 1 stoixeio

        {//general usage
            no_items = items_in_list(b);
            if(no_items-1)
            {
                temp->prev->next= temp->next;
                temp->next->prev = temp->prev;
                free(temp);
            }
            else
            {
                //printf("Only one obj\n");
                b->next=NULL;
                b->prev=NULL;
                free(temp);
            }
        }
        else
        {
            //printf("No such node \n");
        }
        
    }
    else
    {
        printf("This list is Empty\n");
    }
    

}

void free_list(bucket_node *b)  //frees bucket
{
    bucket_node *temp = b;
    int no_items=0;
    no_items= items_in_list(b);
    no_items++;
    while(no_items)
    {
        no_items--;
        temp=b;
        b=b->next;
        free(temp);
    }
}

int find_r_w(bucket_node *b,int pg_num) //finds whether it is a read or write
{
    bucket_node *temp=b;
    if(isEmpty(b) == 0)
    { 
        while(temp->data.pg_num!=pg_num && temp->next->data.pg_num!=-10) //oso den einai afto pou zhtame kai den exoume kanei kuklo
        {
            temp=temp->next;
        }
        if(temp->data.pg_num==pg_num)
        {
            return temp->data.r_w;
        }    
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }    
}

int bucket_min(bucket_node *b)  //find the least recently used page
{
    int min=999999;
    int pg_num;
    bucket_node *temp=b;
    if (isEmpty(b)==0)//to exw valei anapoda
    {
        while(temp->next->data.pg_num!=-10)
        {
            if(temp->data.pos<min)
            {
                min=temp->data.pos;
                pg_num=temp->data.pg_num;
                
            }
            temp=temp->next;
        }
    }
    return pg_num;
    
}

int sca_bucket_min(bucket_node *b)  //finds the page to be replaced by sca
{
    //int min=999999;
    int pg_num=-10;
    bucket_node *temp=b;
    if (isEmpty(b)==0)//to exw valei anapoda
    {
        while(pg_num==-10)
        {    
            temp=b;
            while(temp->next->data.pg_num!=-10)
            {
                if (temp->data.pg_num==-10)
                {
                    temp=temp->next;
                }
                else if(temp->data.sca==1)
                {
                    temp->data.sca=0;
                    temp=temp->next;
                }
                else
                {
                    pg_num=temp->data.pg_num;
                    break;
                }
            }
        }
    }
    return pg_num;
}