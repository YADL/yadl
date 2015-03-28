#include "block.h"
/*Function to create a block.
Input:void
Output:int
*/
int 
creat_block()
{
    
    int fd_blk	=	-1;
    fd_blk = open("fileblockDedup.txt",O_APPEND|O_CREAT|O_RDWR);
    if	(fd_blk< 1)
    {
	printf("\nCreation of block file failed with error [%s]\n",
	strerror(errno));
	goto out;
    }
    printf("\nBlock file created\n");
    out:
    return fd_blk;
}

/*Function to write contents to a block file.
Input:char buff[],size_t l,int fd_blk
Output:int
*/
int 
write_to_block(char buff[],size_t l,int fd_blk)
{
	int ret		=		-1;
	if( write (fd_blk, &l, int_size)== -1)
	{
		printf("\nWrite failed with error%s\n",strerror(errno));
		goto out;
	}
	ret=lseek(fd_blk,1,SEEK_CUR);
	if (ret== -1)
	goto out;
	if (-1 == write(fd_blk,buff,l))
	{
		printf("\nWrite1 failed with error%s\n",strerror(errno));
		goto out;
	}
out:
	return ret;

}

/*Function to read contents from block store.
Input:int fd_blk
Output:int
*/
int 
readBlockTemp(int fd_blk)
{
    
	struct stat		st;
	int	fd2	=		fd_blk;
	int	size	=		0;
	size_t	length	=		0;
	int	b_offset=		0;
	int	e_offset=		0;
	int	ret	=		-1;
	char*	buffer	=		NULL;

	// rewind the stream pointer to the start of temporary file
	if (-1 == lseek(fd2,0,SEEK_SET))
	{
		printf("\nLseek failed with error: [%s]\n",strerror(errno));
		goto out;
	}
	fstat(fd_blk, &st);
	size=st.st_size;
	while (size> 0)
	{
		ret=read(fd2,&length,int_size);
		if (ret== -1)
		{
			printf("\nError while reading %s\n",strerror(errno));
			goto out;
		}
		printf("%d\t\t",length);
		buffer=(char*)calloc(1,length);
		ret = read(fd2,buffer,length);
		if (ret== -1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}
		// Show whatever is read
		buffer[length]='\0';
		printf("%s\n",buffer);
		size-=(length+int_size);
		memset(buffer,0,sizeof(buffer));
	}
	ret=0;
out:
	return ret;
}

/*Function to search block of given range.
Input:int fd_hash,int fd_blk,int fd_stub,int b_offset,int  e_offset
Output:int
*/
int 
searchBlock(int fd_hash,int fd_blk,int fd_stub,int b_offset,int  e_offset)
{
    
	struct stat		st;
	int	size	=		0;
	size_t	length	=		0;
	int	ret	=		-1;
	char*	buffer	=		NULL;
	int p		=		1;
	char* hash	=		NULL;
	int pos		=		0;
	hash=gethash(fd_stub,b_offset,e_offset);
	if (strcmp(hash,"")== 0)
	{
		ret=1;
		printf("\nNo hash present.Please enter a valid range");
		goto out;
	}
	printf("\nSearched hash %s\n",hash);
	pos=getposition(fd_hash,hash);
	if (pos== -1)
	goto out;
	printf("\nPosition is %d\n",pos);
	// rewind the stream pointer to the start of temporary file
	if (-1 == lseek(fd_blk,0,SEEK_SET))
	{
		printf("\nLseek failed with error: [%s]\n",strerror(errno));
		goto out;
	}
	fstat(fd_blk, &st);
	size = st.st_size;
	while (size> 0)
	{
		ret=read(fd_blk,&length,int_size);
		p=p+ret;
		if (ret== -1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		if (p== pos)
		{
			buffer=(char*)calloc(1,length);
		 		ret = read(fd_blk,buffer,length);
			if (ret== -1)
			{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
			}
			// Show whatever is read
			buffer[length]='\0';
			printf("\nBlock from %d to %d is- %s\n",b_offset,e_offset,buffer);
			break;
		}
		buffer=(char*)calloc(1,length);
		ret = read(fd_blk,buffer,length);
		if (ret== -1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}
		p=p+length;
		// Show whatever is read
		buffer[length]='\0';
		size-=(length+int_size);
		memset(buffer,0,sizeof(buffer));
	}
	ret=0;
out:
	return ret;

}

/*Function to get the block from blockstore.
Input:int fd_blk,int pos
Output:char*
*/
char* 
getblock(int fd_blk,int pos)
{
    
	struct stat		st;
	int	size	=		0;
	size_t	length	=		0;
	int	ret	=		-1;
	char*	buffer	=		NULL;
	int 	p	=		1;
	fstat(fd_blk, &st);
	size = st.st_size;
	// rewind the stream pointer to the start of block file
	if (size> 0)
	{
		if (-1 == lseek(fd_blk,0,SEEK_SET))
		{
			printf("\nLseek failed with error: [%s]\n",strerror(errno));
			goto out;
		}
	}
	while (size> 0)
	{
		ret=read(fd_blk,&length,int_size);
		p=p+ret;
		if (ret== -1)
		{
			printf("\nError while reading %s",strerror(errno));
			goto out;
		}
		if (p== pos)
		{
			buffer=(char*)calloc(1,length);
			ret = read(fd_blk,buffer,length);
			if (ret== -1)
			{
			    printf("\nRead failed with error %s\n",strerror(errno));
			    goto out;
			}
			ret=0;
			// Show whatever is read
			buffer[length]='\0';
			break;
		}
		buffer=(char*)calloc(1,length);
		ret = read(fd_blk,buffer,length);
		if (ret== -1)
		{
			printf("\nRead failed with error %s\n",strerror(errno));
			goto out;
		}
		p=p+length;
		// Show whatever is read
		buffer[length]='\0';
		size-=(length+int_size);
		memset(buffer,0,sizeof(buffer));
	}
	ret=0;
out:if (ret== -1)
    {
	memset(buffer,0,sizeof(buffer));
	return buffer;
	}
	else
		return buffer;

}

