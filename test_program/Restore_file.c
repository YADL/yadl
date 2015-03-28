#include "restore.h"
int restore_file()
{
    int fd_cat=-1;
    int ret=-1;
    char* path=NULL;
   fd_cat =open("filecatalog.txt",O_RDONLY);
    if(fd_cat<1)
    {
        printf("\nCreation of catalog file failed with error [%s]\n",
		strerror(errno));
        goto out;
    }
    printf("\ndeduped files\n");
    ret=readfcatalog(fd_cat);
	if(ret==-1)
	{
	goto out;
	}

path=(char*)malloc(sizeof(char *));
out4:printf("\nEnter the exact and full path of dedup file to be restored\n");
		scanf("%s",path);
		ret=comparepath(path,fd_cat);
		if(ret==-1)
		{
			goto out;
		}
		if(ret==1)
		{
			printf("\nPlease enter valid  full path of file");
			goto out4;
		}
		ret=restorefile(path);
	if(ret==-1)
	{
	goto out;
	}
	ret=0;
out:return ret;
}

