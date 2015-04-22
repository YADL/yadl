#include "stdio.h"
#include "catalog.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery/cmockery.h>



/*Function to create catalog file.
Input:void
Output:int*/
int
init_catalog_store()
{
        
        int ret         =       -1;

        fd_cat =open("filecatalog.txt",O_APPEND|O_CREAT|O_RDWR);
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
	check_expected(filename);
        
        if (filename==NULL)
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
	assert(size_of_real_path>0);
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
		assert(length>0);
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
                clean_buff(&buffer);
                ret=1;
        }
       
out:
        return ret;

}
inline void clean_buff(char** buffer)
{
        
        if(*buffer!=NULL)
        {
                free(*buffer);
                *buffer=NULL;
               
        }

}




/*Function to close catalog fd.
Input:void
Output:int*/
int
fini_catalog_store()
{
        
        int ret         =       -1;
        
        assert (fd_cat != -1)
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
