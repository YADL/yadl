#include "block.h"
#include "clean_buff.h"
static struct block_store fd;

/*Function to create block file.
Input:void
Output:int*/
int
init_block_store()
{
        
        int ret         =       -1;

        fd.fd_block =open("blockstore.txt",O_APPEND|O_CREAT|O_RDWR, S_IWUSR|S_IRUSR);
        if (fd.fd_block == -1)
        {
                printf("\nCreation of block file failed with error [%s]\n",
                strerror(errno));
                goto out;
        }
        ret=0;
out:
        return ret;

}

/*Function to write contents to a block file.
Input:char *buffer,size_t length
Output:int
*/
int 
insert_block(char *buffer,size_t length)
{
        
        int ret		=		-1;
        
        if (length<= 0)
        {
                goto out;
        }
        if (write (fd.fd_block, &length, INT_SIZE)== -1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=lseek(fd.fd_block,1,SEEK_CUR);
        if (ret== -1)
                goto out;
        if(buffer == NULL)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if (-1 == write(fd.fd_block,buffer,length))
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
out:
        return ret;

}

/*Function to get the block from blockstore.
Input:int pos
Output:char*
*/
char* 
get_block(int pos)
{
    
        struct stat             st;
        int     size    =               0;
        size_t  length  =               0;
        int     ret     =               -1;
        char*   buffer  =               NULL;
        int     position=               1;

        fstat(fd.fd_block, &st);
        size = st.st_size;
        // rewind the stream pointer to the start of block file
        if (size> 0)
        {
                if (-1 == lseek(fd.fd_block,0,SEEK_SET))
                {
                        printf("\nLseek failed with error: [%s]\n",strerror(errno));
                        goto out;
                }
        }
        while (size> 0)
        {
                ret=read(fd.fd_block,&length,INT_SIZE);
                if (ret== -1)
                {
                        printf("\nError while reading %s",strerror(errno));
                        goto out;
                }
                position=position+ret;
                if (length<= 0)
                {
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);

                if (position== pos)
                {
                      
                        ret = read(fd.fd_block,buffer,length);
                        if (ret== -1)
                        {
                                printf("\nRead failed with error %s\n",strerror(errno));
                                goto out;
                        }
                        ret=0;
                        buffer[length]='\0';
                        break;
                }
               
                ret = read(fd.fd_block,buffer,length);
                if (ret== -1)
                {
                        printf("\nRead failed with error %s\n",strerror(errno));
                        goto out;
                }
                position=position+length;
                buffer[length]='\0';
                size-=(length+INT_SIZE);
                clean_buff(&buffer);

        }
        ret=0;
out:
        if (ret== -1)
        {
                memset(buffer,0,length+1);
        }
        return buffer;

}

/*Function to close block fd.
Input:void
Output:int*/
int
fini_block_store()
{
        
        int ret         =       -1;
        
        if (fd.fd_block != -1)
                ret=close(fd.fd_block);
        if(ret==-1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=0;
out:
        return ret;

}
