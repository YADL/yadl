#include "minhash_stub.h"
#include "catalog.h"
#include "clean_buff.h"
#include "min_hash.h"



/*Function to get the offset of block.
Input:char *hash and int fd_hash
Output:int : offset of block
*/
int
get_minhash_offset(char *hash, int fd_hash)
{

        struct stat             st;
        int     fd2             =               fd_hash;
        int     size            =               0;
        size_t  length          =               0;
        int     ret             =               -1;
        int     offset          =               0;
        char    *buffer         =               NULL;

        fstat(fd_hash, &st);
        size = st.st_size;
        /*rewind the stream pointer to the start of temporary file*/
        if (-1 == lseek(fd_hash, 0, SEEK_SET)) {
                printf("\nLseek failed with error****: [%s]\n", strerror(errno));
                goto out;
        }
        if (size == 0) {
                ret = 1;
                goto out;
        }
        while (size > 0) {
                ret = read(fd2, &length, sizeof(int));
                if (ret == -1) {
                        printf("\nError while reading %s\n", strerror(errno));
                        goto out;
                }
                buffer = (char *)calloc(1, length+1);
                ret = read(fd2, buffer, length);
                if (ret == -1) {
                        printf("\nRead failed with error %s\n",strerror(errno));
                        goto out;
                }
                ret = read(fd2, &offset, sizeof(int));
                if (ret == -1) {
                        printf("\nError while reading %s\n",strerror(errno));
                        goto out;
                }

                buffer[length] = '\0';
                if (strcmp(hash, buffer) == 0) {
                        ret = offset;
                        break;
                }
                size -= (length+sizeof(int)+sizeof(int));

        }
out:
        if(buffer != NULL)
                free(buffer);
        return ret;

}

/*Function to get the block from blockstore.
Input:int pos
Output:char*
*/
char*
get_minhash_block(int pos, int fd_block, int *l)
{

        struct stat                     st;
        int     size     =               0;
        int    length    =               0;
        int     ret      =               -1;
        char    *buffer   =               NULL;
        int     position =               1;

        fstat(fd_block, &st);
        size = st.st_size;
        /*rewind the stream pointer to the start of block file*/
        if (size > 0) {
                if (-1 == lseek(fd_block, 0, SEEK_SET)) {
                        printf("\nLseek failed with error: [%s]\n",strerror(errno));
                        goto out;
                }
        }
        while (size > 0) {
                ret = read(fd_block, &length, sizeof(int));
                *l = length;
                if (ret == -1) {
                        printf("\nError while reading %s",strerror(errno));
                        goto out;
                }
                position = position+ret;
                if (length <= 0) {
                        goto out;
                }
                buffer = (char *)calloc(1, length+1);

                if (position == pos) {
                        ret = read(fd_block, buffer, length);
                        if (ret == -1) {
                                printf("\nRead failed with error %s\n",strerror(errno));
                                goto out;
                        }
                        ret = 0;
                        buffer[length] = '\0';
                        break;
                }
                ret = read(fd_block, buffer, length);
                if (ret == -1) {
                        printf("\nRead failed with error %s\n",strerror(errno));
                        goto out;
                }
                position = position + length;
                buffer[length] = '\0';
                size -= (length+sizeof(int));
                free(buffer);

        }
        ret = 0;
out:
        if (ret == -1) {
                memset(buffer, 0, length+1);
        }
        return buffer;
}


int
init_minhash_stub(char *path, char *filename, int *fd_stub)
{

        int ret         =       -1;
        DIR *dp = NULL;
        char stub_path[1024];

        strcpy(stub_path,path);
        sprintf(stub_path, "%s/store_block/stubs", stub_path);
        dp = opendir(stub_path);
        if (NULL == dp) {
                ret = mkdir(stub_path, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        sprintf (stub_path,"%s/Stub_%s",stub_path, filename);
        *fd_stub = open(stub_path, O_CREAT|O_RDWR,
                S_IWUSR|S_IRUSR);
        if (*fd_stub == -1) {
                printf("\nCreation of stub file failed with error [%s]\n",
                        strerror(errno));
                goto out;
        }
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        return ret;

}

int
delete_minhash_stub(char *path, char *filename)
{

        int ret         =       -1;
        DIR *dp = NULL;
        char stub_path[1024];

        ret = reset_catalog(filename, path);
        if(ret == -1) {
                goto out;
        }
        filename = basename(filename);
        strcpy(stub_path,path);
        sprintf(stub_path, "%s/store_block/stubs", stub_path);
        dp = opendir(stub_path);
        if (NULL == dp) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        sprintf (stub_path,"%s/Stub_%s", stub_path, filename);
        printf("%s", stub_path);
        ret = remove(stub_path);
        if(ret == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        return ret;

}

int
write_to_minhash_stub(void *buff, char *high_similarity_seg,
int threshold_similarity, int similarity, int fd_stub,int buff_counter,
int high_seg_len, char *store_path)
{

        int ret                 =       -1;
        int len                 =        0;
        int b_offset            =        0;
        int length              =        0;
        int pos                 =       -1;
        int l                   =       -1;
        int off                 =       -1;
        int fd_high_hash        =       -1;
        int fd_high_block       =       -1;
        int fd_cur_hash         =       -1;
        int fd_cur_block        =       -1;
        int e_offset            =        0;
        int i                   =        0;
        char *segment_id        =     NULL;
        char *hash              =     NULL;
        char high_seg_block[1024] = "";
        char high_seg_hash[1024]  = "";
        char cur_seg_block[1024]  = "";
        char cur_seg_hash[1024]   = "";
        char *block_buffer        = NULL;
        
        
       
        sprintf(high_seg_block, "%s/store_block/blocks/%s", store_path, high_similarity_seg);
        sprintf(high_seg_hash, "%s_hash", high_seg_block);
        
        if(similarity != 0 || high_similarity_seg != NULL) {
                fd_high_block = open (high_seg_block, O_RDWR|O_APPEND, S_IWUSR|S_IRUSR);
                if (fd_high_block == -1) {
                        printf("\nError in opening %s", high_seg_block);
                        goto out;
                }
                
                fd_high_hash = open (high_seg_hash, O_RDWR|O_APPEND, S_IWUSR|S_IRUSR);
                if (fd_high_hash == -1) {
                        printf("\nError in opening %s",high_seg_hash);
                        goto out;
                }
        }
        while(buff_counter > i) {
                
                memcpy(&len, buff+i, sizeof(int));
                i+=sizeof(int);     
                
                segment_id = (char*)calloc(1, len);
                memcpy(segment_id, buff+i, len);
                i+=len;                        
                
                if(threshold_similarity <= similarity) {
                        if ((i-(sizeof(int)+len)) == 0) {                        
                                sprintf(cur_seg_block, "%s/store_block/blocks/%s",
                                store_path, segment_id);
                                sprintf(cur_seg_hash, "%s_hash", cur_seg_block);
                                
                                fd_cur_hash = open (cur_seg_hash, O_RDONLY, S_IWUSR|S_IRUSR);
                                if (fd_cur_hash == -1) {
                                        printf("\nError in opening %s",strerror(errno));
                                        goto out;
                                }
                                
                                fd_cur_block = open (cur_seg_block, O_RDONLY, S_IWUSR|S_IRUSR);  
                                if (fd_cur_block == -1) {
                                        printf("\nError in opening %s",strerror(errno));
                                        goto out;
                                }
                        }
                        free(segment_id);
                        segment_id = (char*)calloc(1, high_seg_len);
                        memcpy(segment_id, high_similarity_seg, high_seg_len);
                        len = high_seg_len;
                }
                
                if (write (fd_stub, &len, sizeof(int)) == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }               
                if (write (fd_stub, segment_id, strlen(segment_id)) == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }                                
                memcpy(&length, buff+i, sizeof(int));
                if (write (fd_stub, &length, int_size) == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                i+=sizeof(int);
                hash = (char*)calloc(1, length);
                memcpy(hash, buff+i, length);
                if (-1 == write(fd_stub, hash, length)) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                i+=length;

                if(threshold_similarity <= similarity) {
                        
                        ret = search_hash(hash, fd_high_hash);
                        if (ret == -1) {
                                goto out;
                        } else if (ret != 0) {
                                
                                pos = get_minhash_offset(hash, fd_cur_hash);
                                if(pos == -1)
                                        goto out;
                                block_buffer = get_minhash_block(pos, fd_cur_block, &l);
                                if (block_buffer == NULL) {
                                        goto out;
                                }
                                if (write (fd_high_block, &l, int_size) == -1) {
                                        fprintf(stderr, "%s\n", strerror(errno));
                                        goto out;
                                }
                                off = lseek(fd_high_block, 1, SEEK_CUR);
                                if (off == -1)
                                        goto out;
                                ret = write(fd_high_block, block_buffer, l);
                                if (ret < 0) {
                                        printf("error in write");
                                        goto out;
                                }
                        }
                        if(fd_high_hash < 0) {
                                fprintf(stderr, "Error in creating segment hash file\n");
                                goto out;
                        } else {
                                length = strlen(hash);
                                if (write (fd_high_hash, &length, int_size) == -1) {
                                        printf("\nWrite failed with error%s\n",
                                        strerror(errno));
                                        goto out;
                                }
                                if (-1 == write(fd_high_hash, hash, length)) {
                                        printf("\nWrite1 failed with error%s\n",
                                        strerror(errno));
                                        goto out;
                                }
                                if (write (fd_high_hash, &off, int_size) == -1) {
                                        printf("\nWrite failed with error%s\n",
                                        strerror(errno));
                                        goto out;
                                }
                        }
                }

                memcpy(&b_offset, buff+i, sizeof(int));
                if (write (fd_stub, &b_offset, int_size) == -1) {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                i+=sizeof(int);
                memcpy(&e_offset, buff+i, sizeof(int));
                if (write (fd_stub, &e_offset, int_size) == -1) {
                        fprintf(stderr,"%s\n",strerror(errno));
                        goto out;
                }
                i+=sizeof(int);
                free(segment_id);
                free(hash);
        }
        
        if(threshold_similarity <= similarity) {
                ret = remove(cur_seg_block);
                if(ret == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                ret = remove(cur_seg_hash);
                if(ret == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        ret=0;
out:
        if(fd_cur_block)
                close(fd_cur_block);
        if(fd_cur_hash)
                close(fd_cur_hash);
        if(fd_high_block)
                close(fd_cur_block);
        if(fd_high_hash)
                close(fd_cur_hash);
        return ret;

}
