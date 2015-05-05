#include "catalog.h"
#include "clean_buff.h"

// Globals
static size_t fd_cat;

/*@description:Function to check whether path is present or not
@in: char out[]-path,int filedes-file descriptor of file
@out: int 
@return: -1 for error and 0 if found. */
int searchpath(char out[]);

/*Function to create catalog file.
Input:void
Output:int*/
int
init_catalog_store()
{
        
        int ret         =       -1;

        fd_cat =open("filecatalog.txt",O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd_cat< 1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=0;
out:
	return ret;

}

/*Function to write contents to a catalog file.
Input:char* filename
Output:int
*/
int 
writecatalog(char* filename)
{
    
        int ret                 =       -1;
        char actualpath [PATH_MAX+1];
        char *real_path	        =       NULL;
        int size_of_real_path   =       0;
        
        if (filename== '\0' || filename == NULL)
        {
                goto out;
        }
        real_path = realpath(filename, actualpath);
        if (real_path== NULL)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        size_of_real_path=strlen(real_path);
        if (-1 == write(fd_cat,&size_of_real_path,int_size))
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if (-1 == write(fd_cat,real_path,size_of_real_path))
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=0;
out:
        return ret;

}

/*Function to read all deduped files from a catalog file.
Input:void
Output:int
*/
int 
readfilecatalog()
{

        struct stat             st;
        int ret	         =       -1;
        char *buffer     =      NULL;
        int size         =       0;
        int length       =       0;
        
        fstat(fd_cat, &st);
        size = st.st_size;
        if (size> 0)
        {
                if (-1 == lseek(fd_cat,0,SEEK_SET))
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
        }
        else
        {
                goto out;
        }
        printf("\nAbsolute path of deduped files are:\n");
        while (size> 0)
        {
                ret=read(fd_cat,&length,int_size);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);
                ret = read(fd_cat,buffer,length);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer[length]='\0';
               	printf("%s\n",buffer);
                size-=(length+int_size);
                memset(buffer,0,length+1);
                clean_buff(&buffer);
                ret=1;
        }
       
out:
        return ret;

}

/*Function to compare absolute path of file in file catalog.
Input:char out[],int fd_cat
Output:int
*/
int 
comparepath(char out[])
{
        
        struct stat             st;
        int     size    =               0;
        size_t  length  =               0;
        int     ret     =               -1;
        char*   buffer  =               NULL;
        
        fstat(fd_cat, &st);
        size = st.st_size;
        // rewind the stream pointer to the start of catalog file
        if (size> 0)
        {
                if (-1 == lseek(fd_cat,0,SEEK_SET))
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
        }
        if (size== 0)
        {
                ret=1;
                goto out;
        }
        while (size> 0)
        {
                ret=read(fd_cat,&length,int_size);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);
                ret = read(fd_cat,buffer,length);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer[length]='\0';
                if (strcmp(out,buffer)== 0)
                {
                        ret=0;
                        clean_buff(&buffer);
                        break;
                }
                size-=(length+int_size);
                memset(buffer,0,length+1);
                clean_buff(&buffer);
                ret=1;
        }
out:
        return ret;

}

/*Function to close catalog fd.
Input:void
Output:int*/
int
fini_catalog_store()
{
        
        int ret         =       -1;
        
        if (fd_cat != -1)
        ret=close(fd_cat);
        if (ret== -1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=0;
out:
return ret;

}
