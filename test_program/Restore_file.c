#include "restore.h"

/*Function to enter a filename that has to be restored.
Input:void
Output:int
*/
int 
restore_file()
{
        
        int fd_cat      =       -1;
        int ret         =       -1;
        char* path      =       NULL;
        
        printf("\ndeduped files\n");
        ret=readfilecatalog();
        if (ret== -1)
        {
                goto out;
        }
        path=(char*)calloc(1,FILE_SIZE);
        out4:printf("\nEnter the exact and full path of dedup file to be restored\n");
                scanf("%s",path);
                ret=comparepath(path);
                if (ret== -1)
                {
                        goto out;
                }
                if (ret== 1)
                {
                        printf("\nPlease enter valid  full path of file");
                        goto out4;
                }
                ret=restorefile(path);
        if (ret== -1)
        {
        goto out;
        }
        ret=0;
out:
        clean_buff(&path);
        return ret;
        
}

/*Function to delete file and restore it with original contents.
Input:char* path
Output:int
*/
int
restorefile(char* path)
{
        
        int status              =       0;
        int ret                 =      -1;
        char temp_name[NAME_SIZE]    =               "";
        int l                   =       0;
        char *ssc               =       NULL;
        int size                =       0;
        int size1               =       0;
        int     pos             =       0;
        char* buffer            =       NULL;
        char* buffer2           =       NULL;
        char* ptr               =       NULL;
        int length              =       0;
        int sd1	                =       -1;
        int    fd_block         =       -1;
        struct stat             st;
        int bset                =       0;
        int eset                =       0;
        int fd2	                =       -1;
        char actualpath [PATH_MAX+1];
        char* ts1               =       NULL;
        char* ts2               =       NULL;
        char* dir               =       NULL;
        char* filename1         =       NULL;
        
        ts1 = strdup(path);
        ts2 = strdup(path);
        dir = dirname(ts1);
        filename1 = basename(ts2);
        sprintf(dir,"%s/",dir);
        sprintf(temp_name,"%sDedup_%s",dir,filename1);
        printf("%s\n",dir);
        printf("\npath%s",path);
        printf("%s\n",filename1);
        printf("\n%s\n",temp_name);
        sd1 = open(temp_name,O_RDONLY);
        if (sd1< 1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        else
        {
                printf("\nStub file opened\n");
        }

        fstat(sd1, &st);
        size = st.st_size;
        fd2 = open(path,O_CREAT|O_RDWR);
        if (fd2< 1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        else
        {
                printf("\nRestore file created\n");
        }
        if (size> 0)
        {
                if (-1 == lseek(sd1,0,SEEK_SET))
                {
                        fprintf(stderr,"%s\n",strerror(errno));
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
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer=(char*)calloc(1,length+1);
                ret=read(sd1,buffer,length);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                ret=read(sd1,&bset,int_size);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                ret=read(sd1,&eset,int_size);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                buffer[length]='\0';
                pos=getposition(buffer);
                if (pos== -1)
                        goto out;
                printf("\nPosition is %d\n",pos);
                buffer2=get_block(pos);
                if (strcmp(buffer2,"")== 0)
                {
                        goto out;
                }
                printf("block  is %s\n",buffer2);
                ret= write(fd2,buffer2,strlen(buffer2));
                if (ret< 0)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                size1-=(length+int_size+int_size+int_size);
                clean_buff(&buffer);
                clean_buff(&buffer2);
        }
        ret=0;
out:
return ret;

}

