/*This is the main test program*/
#include "main.h"
int main ( int argc, char *argv[] )
{
int chunk_type;
int hash_type;
char filename[100]	=	" ";
int ch                  =       0;
int block_size          =	0;
int fd_cat		=	-1;
int ret			= 	-1;
fd_cat =open("filecatalog.txt",O_APPEND|O_CREAT|O_RDWR); ;
if(fd_cat<1)
{
printf("\nCreation of catalog file failed with error [%s]\n",
		strerror(errno));
	goto out;
}
out1:printf("1.Do you want to dedup a file\n");
printf("2.Do you want restore a file \n");
printf("Enter your choice\n");
scanf("%d" ,&ch);
switch(ch)
{
case 1:
	printf("Please give filname with the full path\n");
	scanf("%s",filename);
	printf("Do you want to do fixed or variable chunking\n");
	out3:printf("1.fixed or 2.variable\n");
	scanf("%d",&chunk_type);
	//printf("%d",chunk_type);
	if(!(chunk_type==1||chunk_type==2))
	{
		printf("Invalid choice please enter valid choice\n");
		goto out3;

	}
	if(chunk_type==2)
	{
		printf("variable chunking is not supported\n");
	}
	printf("Enter block size\n for variable chunking block size should be zero\n");
	scanf("%d",&block_size);
	printf("Choose algorithm to hash\n");
	printf(" 1.md5 2.sha1 3.sha256\n");
	scanf("%d",&hash_type);
	ret=dedup_file(filename,chunk_type,hash_type,block_size);
	if(ret==-1)
		goto out;
	break;
case 2:

	ret= restore_file();
	if(ret==-1)
	goto out;
	break;
default:printf("invalid choice\nEnter valid choice\n ");
	goto out1;
}
ret=0;
out:
 return ret;
}
