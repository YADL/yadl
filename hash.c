#include "hash.h"

int creat_hash()
{
int fd_hash=-1;
fd_hash = open("filehashDedup.txt",O_APPEND|O_CREAT|O_RDWR);
	
	if(fd_hash<1)
	{
		printf("\nCreation of hash file failed with error [%s]\n",
		strerror(errno));
		goto out;
	}
	printf("\nHash file created\n");
out:return fd_hash;
}
/*Function to write contents to a hash file.
Input:char buff[],size_t l,int fd_hash,int offset
Output:int
*/
int write_hash(char buff[],int fd_hash,int offset)
{
	size_t 		l;
	int ret		=		-1;
	l=strlen(buff);
	if(write (fd_hash, &l, int_size)==-1)
	{
		printf("\nWrite failed with error%s\n",strerror(errno));
		goto out;
	}
	if(-1 == write(fd_hash,buff,l))
	{
		printf("\nWrite1 failed with error%s\n",strerror(errno));
		goto out;
	}
	if(write (fd_hash, &offset, int_size)==-1)
	{
		printf("\nWrite failed with error%s\n",strerror(errno));
		goto out;
	}
	ret=0;
out:
	return ret;
}

/*Function to check whether a hash is present in hash store or not.
Input:char out[],int fd_hash
Output:int
*/
int searchhash(char out[],int fd_hash)
{
	struct stat		st;
	int		fd2		=		fd_hash;
	int		size	=		0;
	size_t	length	=		0;
	int		ret		=		-1;
	int		flag	=	1;
	int offset 		=		0;
	char*	buffer	=		NULL;
	fstat(fd_hash, &st);
	size = st.st_size;
	// rewind the stream pointer to the start of temporary file
	if(-1 == lseek(fd_hash,0,SEEK_SET))
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
		ret=read(fd2,&offset,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s\n",strerror(errno));
			goto out;
		}

		buffer[length]='\0';
		if(strcmp(out,buffer)==0)
		{
			ret=0;
			break;
		}
		size-=(length+int_size+int_size);
		memset(buffer,0,sizeof(buffer));
		ret=1;
	}
out:
	return ret;
}

/*Function to get hash of specific block.
Input:int fd_stub,int b_offset,int e_offset
Output:char*
*/
char* gethash(int fd_stub,int b_offset,int e_offset)
{
	struct stat		st;
	int		size	=		0;
	size_t	length	=		0;
	int		ret		=		-1;
	int		flag	=		1;
	int 	eset	=		0;
	int 	bset	=		0;
	char*	buffer	=		NULL;
	fstat(fd_stub, &st);
	size = st.st_size;
	// rewind the stream pointer to the start of temporary file
	if(-1 == lseek(fd_stub,0,SEEK_SET))
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
		ret=read(fd_stub,&length,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		buffer=(char*)calloc(1,length);
		ret = read(fd_stub,buffer,length);
		if(ret==-1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}
		ret=read(fd_stub,&bset,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		ret=read(fd_stub,&eset,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		buffer[length]='\0';
		if(bset==b_offset && eset==e_offset)
		{
			ret=0;
			break;
		}
		size-=(length+int_size+int_size+int_size);
		memset(buffer,0,sizeof(buffer));
		ret=1;
	}
out:if(ret==1)
	{
		memset(buffer,0,sizeof(buffer));
		return buffer;
	}
	else
		return buffer;
}

/*Function to get the position of specific block in hash.
Input:int fd_hash,char* hash
Output:int
*/
int getposition(int fd_hash,char* hash)
{
	struct stat		st;
	int		size	=		0;
	size_t	length	=		0;
	int		ret		=		-1;
	int     flag    =        1;
	int     offset  =		0;
	char*	buffer	=	NULL;
	fstat(fd_hash, &st);
	size = st.st_size;
	// rewind the stream pointer to the start of temporary file
	if(-1 == lseek(fd_hash,0,SEEK_SET))
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
		ret=read(fd_hash,&length,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		buffer=(char*)calloc(1,length);
		ret = read(fd_hash,buffer,length);
		if(ret==-1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}
		ret=read(fd_hash,&offset,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}

		buffer[length]='\0';
		if(strcmp(hash,buffer)==0)
		{
			ret=0;
			break;
		}
		size-=(length+int_size+int_size);
		memset(buffer,0,sizeof(buffer));
		ret=-1;
	}
out:
	if(ret==-1)
		return ret;
	else
		return offset;
}

/*Function to read contents from hash store.
Input:int fd_hash
Output:int
*/
int readTemp(int fd_hash)
{
	struct stat		st;
	int		fd2		=		fd_hash;
	int		size	=		0;
	size_t	length	=		0;
	int		b_offset=		0;
	int		e_offset=		0;
	int		ret		=		-1;
	char*	buffer	=		NULL;

	// rewind the stream pointer to the start of temporary file
	if(-1 == lseek(fd2,0,SEEK_SET))
	{
		printf("\nLseek failed with error: [%s]\n",strerror(errno));
		goto out;
	}

	fstat(fd_hash, &st);
	size = st.st_size;

	while(size>0)
	{
		ret=read(fd2,&length,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s\n",strerror(errno));
			goto out;
		}
		printf("%d\t\t",length);
		buffer=(char*)calloc(1,length);
		ret = read(fd2,buffer,length);
		if(ret==-1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}

		// Show whatever is read
		buffer[length]='\0';
		printf("%s\t",buffer);
		ret=read(fd2,&b_offset,int_size);
		if(ret==-1)
		{
			printf("\nError while reading %s\n",strerror(errno));
			goto out;
		}
		printf("%d\n",b_offset);
		size-=(length+int_size+int_size);
		memset(buffer,0,sizeof(buffer));

	}
	ret=0;

out:
	return ret;
}
