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
    
        filename=(char*)malloc(sizeof(char));
        fd_cat =open("filecatalog.txt",O_RDONLY); 
        if (fd_cat< 1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        printf("\n deduped files are\n");
        ret=readfilecatalog(fd_cat);
        if (ret== -1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        printf("\nSelect the file you want to delete\n");
        scanf("%s",filename);
        printf("\nFILE%s",filename);
        status = remove(filename);
        if( status == 0 )
                printf("\nFile deleted successfully.\n");
        else
        {
                printf("\nUnable to delete the file\n");
                perror("\nError\n");
                goto out;
        }
        ret=0;
out:
return ret;

}
