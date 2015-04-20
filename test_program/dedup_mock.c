#include<cmockery/pbc.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery/cmockery.h>
int searchhash(char* hash)
{
        
        return ((int)mock());

}

int searchstubhash(int fd_stub,int  b_offset,int e_offset)
{
        
        return ((int)mock());

}

int write_to_stub(char *hash,int h_length,int fd_stub,int b_offset,
                     int   e_offset)
{
        
        return ((int)mock());

}

int insert_block(char *buff,int  length)
{
        
        return ((int)mock());

}

int insert_hash(char *hash,int off)
{
        
        return ((int)mock());

}
