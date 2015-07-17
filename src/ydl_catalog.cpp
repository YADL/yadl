#include "ydl_catalog.h"

/*Function to create catalog file.
Input:void
Output:int*/
int
ydl_catalog::init_catalog_store()
{
        int ret         =       -1;

        cat_fd =open("filecatalog.txt",O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (cat_fd< 1)
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
ydl_catalog::writecatalog(char* filename)
{
        int ret                 =       -1;
        char actualpath [PATH_MAX+1];
        char *real_path         =       NULL;
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
        if (-1 == write(cat_fd,&size_of_real_path,INT_SIZE))
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if (-1 == write(cat_fd,real_path,size_of_real_path))
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
ydl_catalog::readfilecatalog()
{

        struct stat             st;
        int ret          =       -1;
        char *buffer     =      NULL;
        int size         =       0;
        int length       =       0;
        
        fstat(cat_fd, &st);
        size = st.st_size;
        if (size> 0)
        {
                if (-1 == lseek(cat_fd,0,SEEK_SET))
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
        }
        else
        {
                goto out;
        }
        cout <<"\nAbsolute path of deduped files are:\n";
        while (size> 0)
        {
                ret=read(cat_fd,&length,INT_SIZE);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);
                ret = read(cat_fd,buffer,length);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer[length]='\0';
                cout << buffer << endl;
                size-=(length+INT_SIZE);
                memset(buffer,0,length+1);
                clean_buff(&buffer);
                ret=1;
        }
       
out:
        return ret;

}

/*Function to compare absolute path of file in file catalog.
Input:char out[],int cat_fd
Output:int
*/
int
ydl_catalog::comparepath(char *out)
{
        
        struct stat             st;
        int     size    =               0;
        size_t  length  =               0;
        int     ret     =               -1;
        char*   buffer  =               NULL;
        
        fstat(cat_fd, &st);
        size = st.st_size;
        // rewind the stream pointer to the start of catalog file
        if (size> 0)
        {
                if (-1 == lseek(cat_fd,0,SEEK_SET))
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
                ret=read(cat_fd,&length,INT_SIZE);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);
                ret = read(cat_fd,buffer,length);
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
                size-=(length+INT_SIZE);
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
ydl_catalog::fini_catalog_store()
{
        
        int ret         =       -1;
        
        if (cat_fd != -1)
        ret=close(cat_fd);
        if (ret== -1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=0;
out:
return ret;

}

