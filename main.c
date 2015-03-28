#include "main.h"
int
main ( int argc, char *argv[] )
{
    
    int chunk_type	=	0;
    int hash_type	=	0;
    char filename[100]	=	" ";
    int ch              =       0;
    int block_size      =	0;
    int fd_cat		=	-1;
    int ret		= 	-1;
    char c;
    while (1)
    {
	out1:printf("\n1.Do you want to dedup a file\n");
	printf("2.Do you want restore a file \n");
	printf("3.delete a file\n");
	printf("4.Exit\n");
	printf("Enter your choice\n");
	scanf("%d" ,&ch);

	switch(ch)
	{
	case 1:
		printf("\nPlease give filname with the full path\n");
		scanf("%s",filename);
		printf("\nDo you want to do fixed or variable chunking\n");
		printf("1.fixed or 2.variable\n");
		while(scanf("%d",&chunk_type))
		{
		    if (!(chunk_type== 1|| chunk_type== 2))
		    {
			printf("\nInvalid choice please enter valid choice\n");
		    }
		    else if (chunk_type== 2)
		    {
			printf("\nVariable chunking is not supported\n");
		    }
		    else
		    {
			break;
		    }
		}
		printf("\nEnter block size\n");
		printf("For variable chunking block size should be zero\n");
		scanf("%d",&block_size);
		printf("\nChoose algorithm to hash\n");
		printf("1.md5 2.sha1\n");
		while(scanf("%d",&hash_type))
		{
		    if (!(hash_type== 1|| hash_type== 2))
		    {
			printf("\nInvalid choice please enter valid choice\n");
		    }
		    else
		    {
			break;
		    }
		}
		ret=dedup_file(filename,chunk_type,hash_type,block_size);
		if (ret== -1)
		    goto out;
		break;
	case 2:
		ret= restore_file();
		if (ret== -1)
		goto out;
		break;
	case 3: ret=delete_file();
		if (ret== -1)
		goto out;
		break;
	case 4: 
		goto out;

	default:printf("\nInvalid choice\nEnter valid choice\n ");
		goto out1;
    }
}
ret=0;
out:
 return ret;

}
