#include "block.h"

/*Function to create block file.
Input:void
Output:int*/
int
init_block_store()
{
        
int ret         =       -1;

        fd.fd_block =open("blockstore.txt",O_APPEND|O_CREAT|O_RDWR);
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
Input:char buff[],size_t length
Output:int
*/
int 
insert_block(char *buff,size_t length)
{
        int ret		=		-1;
        if(length<=0)
        {
                goto out;
        }
        if( write (fd.fd_block, &length, INT_SIZE)== -1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=lseek(fd.fd_block,1,SEEK_CUR);
        if (ret== -1)
                goto out;
        if(buff == NULL)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if (-1 == write(fd.fd_block,buff,length))
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
out:
    	clean_buff(&buff);
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
                if(length<=0)
                {
                        goto out;
                }
                if (position== pos)
                {
                        buffer=(char*)calloc(1,length);
                        ret = read(fd.fd_block,buffer,length);
                        if (ret== -1)
                        {
                                printf("\nRead failed with error %s\n",strerror(errno));
                                goto out;
                        }
                        ret=0;
                        // Show whatever is read
                        buffer[length]='\0';
                        break;
                }
                buffer=(char*)calloc(1,length);
                ret = read(fd.fd_block,buffer,length);
                if (ret== -1)
                {
                        printf("\nRead failed with error %s\n",strerror(errno));
                        goto out;
                }
                position=position+length;
                // Show whatever is read
                buffer[length]='\0';
                size-=(length+INT_SIZE);
                memset(buffer,0,sizeof(buffer));
        }
        ret=0;
out:
        if (ret== -1)
        {
                memset(buffer,0,sizeof(buffer));
        }
        return buffer;

}

/*Function to close block fd.
Input:void
Output:int*/
int
fini_block_store()
{
int ret=-1;
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
