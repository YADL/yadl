#include "clean_buff.h"
#include "delete.h"

/*Function to delete file from a directory.
Input:void
Output:int 
*/
int
delete_file()
{
    
        int fd_cat      =       -1;
        int ret	        =       -1;
        int status      =       -1;
        char *filename  =       NULL;
    
        filename=(char*)calloc(1,FILE_SIZE);
        fd_cat =open("filecatalog.txt",O_RDONLY); 
        if (fd_cat< 1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        printf("\n deduped files are\n");
        ret=readfilecatalog();
        if (ret== -1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        printf("\nSelect the file you want to delete\n");
        while (scanf("%s",filename) <= 0);
        printf("\nFILE%s",filename);
        status = remove(filename);
        if( status== 0 )
                printf("\nFile deleted successfully.\n");
        else
        {
                printf("\nUnable to delete the file\n");
                perror("\nError\n");
                goto out;
        }
        ret=0;
out:
        clean_buff(&filename);
return ret;

}
