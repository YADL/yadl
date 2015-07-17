#include <iostream>
using namespace std;
#include<stdio.h>
#include<string.h>  
#include<stdlib.h> 
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<ctype.h> 
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include "vector.h"
#include "clean_buff.h"
#ifndef YDL_BLOCK_H
#define YDL_BLOCK_H
#define INT_SIZE sizeof(int)

class ydl_block
{
        static int block_fd;
public:
        int init_block_store();
        char* get_block(int pos, int *l);
        int insert_block(vector_ptr list, size_t length);
        int fini_block_store();
};

int ydl_block::block_fd;
#endif // YDL_BLOCK_H
