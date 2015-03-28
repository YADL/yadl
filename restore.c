#include "restore.h"
/*Function to delete file and restore it with original contents.
Input:char* path,int fd_hash,int fd_block
Output:int
*/
int restorefile(char* path)
{
	int	status		=	0;
	int	ret			=	-1;
	char	temp_name[NAME_SIZE]	=		"";
	int l 			= 	0;
	char *ssc		=	NULL;
	int size		=	0;
	int size1		=	0;
	int	 pos		=	0;
	char* buffer1	=	NULL;
	char* buffer2	=	NULL;
	char* ptr		=	NULL;
	int length		=	0;
	int sd1			=	-1;
	int fd_hash     =   -1;
	int    fd_block =   -1;
	struct stat		st;
	int bset		=	0;
	int eset		=	0;
	int fd2			=	-1;
	char actualpath [PATH_MAX+1];
	char* ts1		=	NULL;
	char* ts2		=	NULL;
	char* dir		=	NULL;
	char* filename1	=	NULL;
   	ts1 = strdup(path);
	ts2 = strdup(path);
	dir = dirname(ts1);
	filename1 = basename(ts2);
	sprintf(dir,"%s/",dir);
	sprintf(temp_name,"%sDedup_%s",dir,filename1);

	sd1 = open(temp_name,O_RDONLY);
	if(sd1<1)
	{
		printf("\nOpening of stub file failed with error [%s]\n",
		strerror(errno));
		goto out;
	}
	else
	{
		printf("\nStub file opened\n");
	}
	fd_hash =open("hashstore.txt",O_APPEND|O_CREAT|O_RDWR);
	if(fd_hash<1)
	{
		printf("\nCreation of hash store failed with error [%s]\n",
		strerror(errno));
		goto out;
	}
	fd_block =open("blockstore.txt",O_APPEND|O_CREAT|O_RDWR);
	if(fd_block<1)
	{
		printf("\nCreation of block file failed with error [%s]\n",
		strerror(errno));
		goto out;
	}
	fstat(sd1, &st);
	size = st.st_size;
	fd2 = open(path,O_CREAT|O_RDWR);
	if(fd2<1)
	{
		printf("\nCreation of restore file failed with error [%s]\n",
		strerror(errno));
		goto out;
	}
	else
	{
		printf("\nRestore file created\n");
	}
	if(size>0)
	{
		if(-1 == lseek(sd1,0,SEEK_SET))
		{
			printf("\nLseek failed with error: [%s]\n",strerror(errno));
			goto out;
		}
	}
	if(size==0)
	{
		printf("\nNo contents\n");
		ret=-1;
		goto out;
	}
	while(size>0)
	{
		ret=read(sd1,&length,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		buffer1=(char*)calloc(1,length);
		ret=read(sd1,buffer1,length);
		if(ret==-1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}
		ret=read(sd1,&bset,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		ret=read(sd1,&eset,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		buffer1[length]='\0';
		pos=getposition(fd_hash,buffer1);
		if(pos==-1)
			goto out;
		printf("\nPosition is %d\n",pos);
		buffer2=getblock(fd_block,pos);
		if(strcmp(buffer2,"")==0)
		{
			goto out;
		}
		ret= write(fd2,buffer2,strlen(buffer2));
		if(ret<0)
		{
			printf("\nWrite1 failed with error%s\n",strerror(errno));
			goto out;
		}
		size1-=(length+int_size+int_size+int_size);
		memset(buffer1,0,sizeof(buffer1));
	}
	ret=0;
out:
return ret;
}

