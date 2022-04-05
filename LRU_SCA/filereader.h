#ifndef FILEREADER_H
#define FILEREADER_H
#include "hash.h"
int read_p(FILE *trace,int q,char* method,int *num_of_reads,int *num_of_writes,int *end_flag,bucket_node **buckets,int *avail_mem,int *pos,bucket_node **buckets_other\
,bucket_node *combine,int *pg_fault);

#endif