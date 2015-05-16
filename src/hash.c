#include "hash.h"
#include "clean_buff.h"

static int fd_hash;

/*Function to create hash for a given block
Input:void
Output:int*/
int 
init_hash_store()
{
        
        int ret         =       -1;
        
        fd_hash = open("filehashDedup.txt",O_APPEND|O_CREAT|O_RDWR,
                        S_IRUSR|S_IWUSR);
        if (fd_hash == -1)
        {
        log_write(LOG_ERROR,"Init hash store",
                "\nCreation of hash file failed with error [%s]\n",
                strerror(errno));
        goto out;
        }
        ret=0;

out:
        return ret;

}

/*Function to write contents to a hash file.
Input:char *buff,int offset
Output:int
*/
int 
insert_hash(char *buff,int offset)
{
    
        size_t          length;
        int ret         =               -1;

        length=strlen(buff);
        if (write (fd_hash, &length, int_size)== -1)
        {
                log_write(LOG_ERROR,"Insert hash",
                        "\nWrite failed with error %s\n",
                        strerror(errno));
                goto out;
        }
        if (-1 == write(fd_hash,buff,length))
        {
                log_write(LOG_ERROR,"Insert hash",
                        "\nWrite1 failed with error%s\n",
                        strerror(errno));
                goto out;
        }
        if (write (fd_hash, &offset, int_size)== -1)
        {
                log_write(LOG_ERROR,"Insert hash",
                        "\nWrite failed with error%s\n",
                        strerror(errno));
                goto out;
        }
        ret=0;
out:
        return ret;

}

/*Function to check whether a hash is present in hash store or not.
Input:char *out
Output:int
*/
int 
searchhash(char *out)
{
    
        struct stat             st;
        int     fd2             =               fd_hash;
        int     size            =               0;
        size_t  length          =               0;
        int     ret             =               -1;
        int offset              =               0;
        char*   buffer          =               NULL;
        
        fstat(fd_hash, &st);
        size = st.st_size;
        // rewind the stream pointer to the start of temporary file
        if (-1 == lseek(fd_hash,0,SEEK_SET))
        {
                log_write(LOG_ERROR,"Search hash",
                        "\nLseek failed with error: [%s]\n",
                        strerror(errno));
                goto out;
        }
        if (size== 0)
        {
                ret=1;
                goto out;
        }
        while (size> 0)
        {
                ret=read(fd2,&length,int_size);
                if (ret== -1)
                {
                        log_write(LOG_ERROR,"Search hash",
                                "\nError while reading %s\n",
                                strerror(errno));
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);
                ret = read(fd2,buffer,length);
                if (ret== -1)
                {
                        log_write(LOG_ERROR,"Search hash",
                                "\nRead failed with error %s\n",
                                strerror(errno));
                        goto out;
                }
                ret=read(fd2,&offset,int_size);
                if (ret== -1)
                {
                        log_write(LOG_ERROR,"Search hash",
                                "\nError while reading %s\n",
                                strerror(errno));
                        goto out;
                }

                buffer[length]='\0';
                if (strcmp(out,buffer)== 0)
                {
                        ret=0;
                        clean_buff(&buffer);
                        break;
                }
                size-=(length+int_size+int_size);
                clean_buff(&buffer);
                ret=1;
        }
out:
        return ret;

}

/*Function to get the position of specific block in hash.
Input:char* hash
Output:int
*/
int 
getposition(char* hash)
{
    
        struct stat             st;
        int     size    =       0;
        size_t  length  =       0;
        int     ret     =       -1;
        int     offset  =       0;
        char*   buffer  =       NULL;
        int h_length    =       0;
        
        fstat(fd_hash, &st);
        size = st.st_size;
        h_length=strlen(hash);
        // rewind the stream pointer to the start of temporary file
        if (-1 == lseek(fd_hash,0,SEEK_SET))
        {
                log_write(LOG_ERROR,"Get position",
                        "\nLseek failed with error: [%s]\n",
                        strerror(errno));
                goto out;
        }
        if (size== 0)
        {
                ret=1;
                goto out;
        }
        
        while (size> 0)
        {
                ret=read(fd_hash,&length,int_size);
                if (ret== -1)
                {
                        log_write(LOG_ERROR,"Get position",
                                "\nError while reading %s",
                                strerror(errno));
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);
                hash[h_length]='\0';
                ret = read(fd_hash,buffer,length);
                if (ret== -1)
                {
                        log_write(LOG_ERROR,"Get position",
                                "\nRead failed with error %s\n",
                                strerror(errno));
                        goto out;
                }
                ret=read(fd_hash,&offset,int_size);
                if (ret== -1)
                {
                        log_write(LOG_ERROR,"Get position",
                                "\nError while reading %s",
                                strerror(errno));
                        goto out;
                }
                buffer[length]='\0';
                if (strcmp(hash,buffer)== 0)
                {
                        ret=0;
                        clean_buff(&buffer);
                        break;
                }
                size-=(length+int_size+int_size);
                clean_buff(&buffer);
                ret=-1;
        }
out:
        if (ret== -1)
                return ret;
        else
                return offset;

}



/*Function to close hash fd.
Input:void
Output:int*/
int
fini_hash_store()
{
        
        int ret         =       -1;
        
        if (fd_hash != -1)
                ret=close(fd_hash);
        if(ret==-1)
        {
                log_write(LOG_ERROR,"Fini hash store",
                        "%s\n",strerror(errno));
                goto out;
        }
        ret=0;

out:
        return ret;

}
