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
#include<time.h> 
#include <libgen.h> 
#include <stdarg.h>
#include<stdlib.h> 
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery/cmockery.h>
#include <cmockery/cmockery_override.h>
int searchhash(char* hash);
int searchstubhash(int fd_stub,int  b_offset,int e_offset);
int write_to_stub(char *hash,int h_length,int fd_stub,int b_offset,
int   e_offset);
int insert_block(char *buff,int  length);
int insert_hash(char *hash,int off);
int
chunk_store(char *buff,char *hash,int length,int h_length,int b_offset,
int e_offset,int fd_stub);
