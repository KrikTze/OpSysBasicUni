#ifndef HASH_H
#define HASH_H
#define NUM_BUCKETS 10


//It is a doubly linked circular list
//Used to create the hash table
typedef struct bucket_item bucket_item;


struct bucket_item{
    int pos;
    int pg_num;
    int r_w;
    int sca;


}typedef bucket_item;

typedef struct bucket_node bucket_node;

struct bucket_node
{
    bucket_item data;
    bucket_node *next;
    bucket_node *prev;
};

int hash_funct(int num);
void bucket_init(bucket_node **bucket);
int isEmpty(bucket_node *b);
void bucket_add(bucket_node *b,int pg_num,int pos,int r_W,int sca);
void list_print(bucket_node *b);
int items_in_list(bucket_node *b);
void list_del_item(bucket_node *b,int pg_num);
void free_list(bucket_node *b);
int bucket_find(bucket_node *b,int pg_num);
void bucket_update(bucket_node *b,int pg_num,int pos,int r_w,int sca);
int find_r_w(bucket_node *b,int pg_num);
int sca_bucket_min(bucket_node *b);
int bucket_min(bucket_node *b);


#endif