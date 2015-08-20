#include "vector.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>

/*@description:Function to insert block to blockstore object
@in: vector_ptr list-buffer containing block,size_t length-size of block
@out: int 
@return: -1 for error and 0 if inserted successfully */
int insert_block_to_object(char *hash,vector_ptr list, char *store_path);

/*@description:Function to get specific block from object
@in: char *hash - hash of block
@out: char*
@return: block */
char *get_block_from_object(char *hash,int *length, char *store_path);
