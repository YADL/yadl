#include "stub.h"

/*Function to write contents to a stub file.
Input:char buff[],size_t l,int fd_stub,int b_offset,int e_offset
Output:int
*/
int
write_to_stub(char buff[],size_t length,int fd_stub,int b_offset,int e_offset)
{
        
        int ret	        =        -1;
        REQUIRE(buff!=NULL);
        REQUIRE(length>0);
        REQUIRE(fd_stub>0);

        if(write (fd_stub, &length, int_size)==-1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if(-1 == write(fd_stub,buff,length))
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if(write (fd_stub, &b_offset, int_size)==-1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if(write (fd_stub, &e_offset, int_size)==-1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=0;
out:
        ENSURE (ret== 0);
        return ret;

}

/*Function to check whether a hash of specfic range is present in stub or not.
Input:int fd_stub,int b_offset,int e_offset
Output:int
*/
int
searchstubhash(int fd_stub,int b_offset,int e_offset)
{
        
        struct stat             st;
        int    size             =               0;
        size_t  length          =               0;
        int     ret             =              -1;
        int eset                =               0;
        int bset                =               0;
        char*   buffer          =               NULL;
        
        REQUIRE(fd_stub>0);
        fstat(fd_stub, &st);
        size = st.st_size;
        // rewind the stream pointer to the start of stub file
        if (size> 0)
        {
                if(-1 == lseek(fd_stub,0,SEEK_SET))
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
        }
        if (size== 0)
        {
                ret     =       1;
                goto out;
        }
        while (size> 0)
        {
                ret=read(fd_stub,&length,int_size);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);
                ret = read(fd_stub,buffer,length);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                ret=read(fd_stub,&bset,int_size);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                ret=read(fd_stub,&eset,int_size);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer[length]='\0';
                if (bset== b_offset && eset== e_offset)
                {
                        ret=0;
                        clean_buff(&buffer);
                        break;
                }
                size-=(length+int_size+int_size+int_size);
                clean_buff(&buffer);
                ret=1;
        }
out:
        return ret;

}

