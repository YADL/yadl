#include "catalog.h"
int creatcatalog()
{
int fd_cat=-1;
fd_cat = open("filecatalog.txt",O_APPEND|O_CREAT|O_RDWR);

	if(fd_cat<1)
	{
		printf("\nCreation of catalog file failed with error [%s]\n",
		strerror(errno));
		goto out;
	}
out:return fd_cat;
}
/*Function to write contents to a catalog file.
Input:int fd_cat,char* argv
Output:int
*/
int writecatalog(int fd_cat,char* argv)
{
	int ret					=	-1;
	char actualpath [PATH_MAX+1];
	char *real_path			=	NULL;
	int size_of_real_path	=	0;

	real_path = realpath(argv, actualpath);
	size_of_real_path=strlen(real_path);

	if(-1 == write(fd_cat,&size_of_real_path,int_size))
	{
		printf("\nWrite1 failed with error%s\n",strerror(errno));
		goto out;
	}
	if(-1 == write(fd_cat,real_path,size_of_real_path))
	{
		printf("\nWrite1 failed with error%s\n",strerror(errno));
		goto out;
	}
	ret=0;
out:
	return ret;
}
/*Function to check whether a path is already present in file catalog or not.
Input:char out[],int fd_cat
Output:int
*/
int searchpath(char out[],int fd_cat)
{

	struct stat		st;
	int		fd2		=	fd_cat;
	int		size	=		0;
	size_t	length	=		0;
	int		ret		=		-1;
	char*	buffer	=	NULL;
	fstat(fd_cat, &st);
	size = st.st_size;
	// rewind the stream pointer to the start of catalog file
	if(-1 == lseek(fd_cat,0,SEEK_SET))
	{
		printf("\nLseek failed with error: [%s]\n",strerror(errno));
		goto out;
	}
	if(size==0)
	{
		ret=1;
		goto out;
	}

	while(size>0)
	{
		ret=read(fd2,&length,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s\n",strerror(errno));
			goto out;
		}
		buffer=(char*)calloc(1,length);
		ret = read(fd2,buffer,length);
		if(ret==-1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}
		buffer[length]='\0';
		if(strcmp(out,buffer)==0)
		{
			ret=-1;
			break;
		}
		size-=(length+int_size);
		memset(buffer,0,sizeof(buffer));
		ret=1;
	}

out:
	return ret;
}
/*Function to read all deduped files from a catalog file.
Input:int fd_cat
Output:int
*/
int readfcatalog(int fd_cat)
{
	struct stat		st;
	int ret			=	-1;
	char *buffer2	=	NULL;
	int size		=	0;
	fstat(fd_cat, &st);
	int l			=	0;
	size = st.st_size;
	if(size>0)
	{
		if(-1 == lseek(fd_cat,0,SEEK_SET))
		{
			printf("\nLseek failed with error: [%s]\n",strerror(errno));
			goto out;
		}
	}
	else
	{
		goto out;
	}
	printf("\nAbsolute path of deduped files are:");
	while(size>0)
	{
	ret=read(fd_cat,&l,int_size);
	if(ret==-1)
	{
		printf("\nError while reading %s",strerror(errno));
		goto out;
	}
	buffer2=(char*)calloc(1,l);
	ret = read(fd_cat,buffer2,l);

	if(ret==-1)
	{
		printf("\nRead failed with error %s\n",strerror(errno));
		goto out;
	}
	printf("\n%s",buffer2);
	ret=0;
	size-=(l+int_size);
	}
out:
	return ret;
}

/*Function to compare absolute path of file in file catalog.
Input:char out[],int fd_cat
Output:int
*/
int comparepath(char out[],int fd_cat)
{
	struct stat		st;
	int		fd2		=		fd_cat;
	int		size	=		0;
	size_t	length	=		0;
	int		ret		=		-1;
	int		flag	= 		1;
	char*	buffer	=		NULL;
	fstat(fd_cat, &st);
	size = st.st_size;
	// rewind the stream pointer to the start of catalog file
	if(size>0)
	{
		if(-1 == lseek(fd_cat,0,SEEK_SET))
		{
			printf("\nLseek failed with error: [%s]\n",strerror(errno));
			goto out;
		}
	}
	if(size==0)
	{
		ret=1;
		goto out;
	}

	while(size>0)
	{
		ret=read(fd2,&length,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		buffer=(char*)calloc(1,length);
		ret = read(fd2,buffer,length);
		if(ret==-1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}
		buffer[length]='\0';
		if(strcmp(out,buffer)==0)
		{
			ret=0;
			break;
		}
		size-=(length+int_size);
		memset(buffer,0,sizeof(buffer));
		ret=1;
	}
out:
	return ret;
}
