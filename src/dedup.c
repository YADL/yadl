#include "dedup.h"
#include "clean_buff.h"
#include "main.h"
#include "Rabin_Karp.h"

#define NAME_SIZE 100

// From catalog.h
int comparepath(char out[]);

// Prototypes

/*@description:Function to check whether hash is already present or not.
@in: char *out-input hash
@out: int hash
@return: -1 for error and 0 if hash already present */
int searchhash(char *out);
int get_next_chunk(int fd_input,int chunk_type,
        int block_size,char** buffer, int *length);
int chunk_store(char *buff,char *hash,int length,
        int h_length,int e_offset, int b_offset,int fd_stub);
/*
Function to dedup a file whose path is specified by the user.
Input:char* filename,int chunk_type,int hash_type,int block_size
Output:int
*/
int
dedup_file (char* filename,int chunk_type,int hash_type,int block_size)
{

        int ret                 =       -1;
        int fd_input            =       -1;
        int fd_stub             =       -1;
        int length              =       0;
        int h_length            =       0;
        int b_offset            =       0;
        int e_offset            =       0;
        int size                =       0;
        char* hash              =       NULL;
        char* ts1               =       NULL;
        char* ts2               =       NULL;
        char* dir               =       NULL;
        char* filename1         =       NULL;
        char temp_name[NAME_SIZE]=      "";
        char* buffer            =       NULL;
        char* chunk_buffer      =       NULL;
        FILE *fp                =       NULL;
        struct stat st;
        
        ts1 = strdup(filename);
        ts2 = strdup(filename);
        dir = dirname(ts1);
        filename1 = basename(ts2);
        sprintf(dir,"%s/",dir);
        sprintf(temp_name,"%sDedup_%s",dir,filename1);
        fd_input =open(filename,O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd_input< 1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=comparepath(filename);
        if (ret== -1)
        {
                goto out;
        }
        if (ret== 0)
        {
                printf("\nfile is already deduped");
                goto out;
        }
        fd_stub =open(temp_name,O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd_stub< 1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        fstat(fd_input, &st);
        size = st.st_size;
        if(chunk_type == 1)
        {
                while(1)
                {
                        if (size<= block_size)
                        {
                                block_size=size;
                        }
                        b_offset=e_offset;
                        ret=get_next_chunk(fd_input, chunk_type,block_size,
                        &buffer, &length);
                        if (ret<= 0)
                                break;
                        e_offset+=length-1;
                        size=size-length;
                        ret=get_hash(buffer,length, hash_type,&hash,&h_length);
                        if (ret== -1)
                                goto out;
                        ret=chunk_store(buffer,hash,length,h_length,
                        b_offset,e_offset,fd_stub);
                        if (ret== -1)
                                goto out;
                        e_offset++;
                        if (size<= 0)
                        {
                                ret=0;
                                break;
                        }
                        clean_buff(&buffer);
                        clean_buff(&hash);
                }
        }
        else
        {
                fp      = fopen("./Rabin_Karp.csv","w+");
                if(fp == NULL)
                {
                        fprintf(stderr, 
                                "Error in creating file Rabin_Karp.csv\n");
                        goto out;
                }

                while(1)
                {
                        b_offset=e_offset;
                        chunk_buffer = get_variable_chunk(fd_input,
                                &ret,&size);
                        if(ret == -1)
                        {
                                fprintf (stderr, 
                                        "Error in variable chunking\n");
                                goto out;
                        }
                        length = strlen(chunk_buffer);
                        e_offset+=length-1;
                        ret=get_hash(chunk_buffer,length, 
                                hash_type,&hash,&h_length);
                        if (ret== -1)
                                goto out;
                        ret=chunk_store(chunk_buffer,hash,length,
                                h_length,b_offset,e_offset,fd_stub);
                        if (ret== -1)
                                goto out;
                        e_offset++;
                        clean_buff(&chunk_buffer);
                        if(size == 0)
                                break;
                }
        }
        ret=writecatalog(filename);
        if (ret== -1)
        {
                goto out;
        }
        ret=0;
out:
        if(fp != NULL)
                fclose(fp);
        if(fd_input != -1)
                close(fd_input);
        return ret;
}

/*
Function to get next chunk from file.
Input:int fd_input,int chunk_type,int block_size,char** buffer,
int *length
Output:int
*/
int 
get_next_chunk(int fd_input,int chunk_type,int block_size,char** buffer,
int *length)
{

        int ret         =       -1;
        
        *buffer=(char *)calloc(1,block_size+1);
        ret=read(fd_input,*buffer,block_size);
        if (ret< 1)
                goto out;
        *length=strlen(*buffer);
out:
        return ret;

}

/*
Function to get hash from a specific algorithm.
Input:char *buffer,int length,int hash_type,char** hash,int *h_length
Output:int
*/
int 
get_hash(char *buffer,int length,int hash_type,char** hash,int *h_length)
{
        
        int ret         =       -1;
        char *buf       =       NULL;
        char *buff      =       NULL;

        switch(hash_type)
        {
                case 1:
                        buf = str2md5(buffer, length);
                        *hash = buf;
                        *h_length=strlen(buf);;
                        break;
                case 2:
                        buff =sha1(buffer, length);
                        *hash = buff;
                        *h_length=strlen(buff);
                        break;
        }
        ret=0;
        return ret;

}

/*
Function to store chunks in chunk store and hash in hash store.
Input:char *buff,char *hash,int length,int h_length,int e_offset,
int b_offset,int fd_stub
Output:int
*/
int 
chunk_store(char *buff,char *hash,int length,int h_length,int e_offset,
int b_offset,int fd_stub)
{

        int off                 =       -1;
        int ret                 =       -1;

        ret=searchhash(hash);
        if( ret== -1)
        {
                goto out;
        }
        if (ret== 0)
        {
                ret=searchstubhash(fd_stub, b_offset,e_offset);
                if (ret== -1)
                        goto  out;
                if (ret== 1)
                {
                        ret=write_to_stub(hash,h_length,fd_stub,b_offset,
                        e_offset);
                        if (ret== -1)
                                goto out;
                }
        }
        else
        {
                //scanf("%d",&off);
                off=insert_block(buff, length);
                if (off== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                ret=insert_hash(hash,off);
                if (ret== -1)
                {
                        fprintf(stderr,"%s\n",strerror(errno));;
                        goto out;
                }
                ret=searchstubhash(fd_stub, b_offset,e_offset);
                if (ret== -1)
                        goto  out;
                if (ret== 1)
                {
                        ret=write_to_stub(hash,h_length,fd_stub,b_offset,
                        e_offset);
                        if (ret== -1)
                        {
                                fprintf(stderr,"%s\n",strerror(errno));
                                goto out;
                        }
                }
        }
        ret=0;
out:
        return ret;

}





