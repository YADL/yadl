#include "delete.h"
/*Function to delete file from a directory.
Input:void
Output:int 
*/
int
delete_file()
{
    
    int fd_cat	=	-1;
    int ret	=	-1;
    int status	=	-1;
    char *filename	=	NULL;
    filename=(char*)malloc(sizeof(char));
    fd_cat =open("filecatalog.txt",O_RDONLY); 
    if (fd_cat< 1)
    {
    printf("\nopenning of catalog file failed with error [%s]\n",
		strerror(errno));
    goto out;
    }
    printf("\n deduped files are\n");
    ret=readfcatalog(fd_cat);
    if (ret== -1)
    {
	printf("\nReading of catalog file failed with error [%s]\n",
	strerror(errno));
	goto out;
    }
    printf("select the file you want to delete\n");
    scanf("%s",filename);
    printf("\nFILE%s",filename);
    status = remove(filename);
    if( status == 0 )
	printf("File deleted successfully.\n");
    else
    {
	printf("Unable to delete the file\n");
	perror("Error");
	goto out;
    }
    ret=0;
out:
return ret;

}
