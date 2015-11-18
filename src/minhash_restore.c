#include "minhash_restore.h"
#include "catalog.h"
#include "minhash_stub.h"

/* Function to restore it with original contents.
Input   :  char* path
Output  :  int
*/
int
minhash_restore(char *path)
{

        char file[100]          =       "";
        char seg_file[50]       =       "";
        char *seg_buffer        =       NULL;
        char *hash_buffer       =       NULL;
        char *block_buffer      =       NULL;
        char *ts1               =       NULL;
        char *filename          =       NULL;
        int fd_input            =       -1;
        int fd_hash             =       -1;
        int fd_block            =       -1;
        int length              =        0;
        int pos                 =       -1;
        int l                   =       -1;
        int ret                 =       -1;
        ssize_t size            =        0;
        ssize_t b_offset        =        0;
        ssize_t e_offset        =        0;
        struct stat             st;
        int fd_output           =       -1;

        ret = comparepath(path);
        if(ret == -1) {
                goto out;
        }
        fd_output = open (path, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
        if(fd_output < 0) {
                fprintf(stderr, "Error in opening output file\n");
                goto out;
        }

        ts1 = strdup(path);
        filename = basename(ts1);
        printf("Restore in progress...\n");
        ret = init_minhash_stub("/var/lib", filename, &fd_input);
        if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        
        if(fd_input < 0) {
                fprintf(stderr, "Error in opening input file %s\n", file);
                goto out;
        } else {
                fstat(fd_input, &st);
                size = st.st_size;
                if (size == 0) {
                        ret = 1;
                        goto out;
                }
                while (1) {
                        ret = read(fd_input, &length, sizeof(int));
                        if (ret == -1) {
                                printf("\nError while reading ");
                                goto out;
                        }
                        seg_buffer = (char *)calloc(1, length+1);
                        ret = read(fd_input, seg_buffer, length);
                        if (ret == -1) {
                                printf("\nRead failed with error");
                                goto out;
                        }
                        size -= length+sizeof(int);
                        ret = read(fd_input, &length, sizeof(int));
                        if (ret == -1) {
                                printf("\nError while reading ");
                                goto out;
                        }
                        hash_buffer = (char *)calloc(1, length+1);
                        ret = read(fd_input, hash_buffer, length);
                        if (ret == -1) {
                                printf("\nRead failed with error");
                                goto out;
                        }
                        size -= length+sizeof(int);

                        ret = read(fd_input, &b_offset, sizeof(int));
                        if (ret == -1) {
                                printf("\nError while reading ");
                                goto out;
                        }
                        ret = read(fd_input, &e_offset, sizeof(int));
                        if (ret == -1) {
                                printf("\nError while reading ");
                                goto out;
                        }
                        size -= sizeof(int) + sizeof(int);
                        sprintf(seg_file, "/var/lib/store_block/blocks");
                        sprintf(seg_file, "%s/%s", seg_file, seg_buffer);
                        fd_block = open (seg_file, O_RDONLY, S_IRUSR|S_IWUSR);
                        if(fd_block < 0) {
                                fprintf(stderr, "Error in opening block file\n");
                                goto out;
                        } else {
                                sprintf(seg_file, "%s_hash", seg_file);
                                fd_hash = open (seg_file, O_RDONLY, S_IRUSR|S_IWUSR);
                                if(fd_hash < 0) {
                                        fprintf(stderr,
                                        "Error in opening block file %s\n", seg_file);
                                        goto out;
                                }
                                pos = get_minhash_offset(hash_buffer, fd_hash);
                                if(pos == -1)
                                        goto out;
                                block_buffer = get_minhash_block(pos, fd_block, &l);
                                if (block_buffer == NULL) {
                                        goto out;
                                } else {
                                        ret = write(fd_output, block_buffer, l);
                                        if (ret < 0) {
                                                printf("error in write");
                                                goto out;
                                        }
                                }
                        }
                        if(hash_buffer != NULL)
                                free(hash_buffer);
                        if(seg_buffer != NULL)
                                free(seg_buffer);
                        memset(seg_file, 0, 50);
                        if(fd_block)
                                close(fd_block);
                        if(fd_hash)
                                close(fd_hash);
                        
                        if(size == 0)
                                break;
                }
        }
        ret = 0;
out:
        if(fd_input)
                close(fd_input);
        if(fd_output)
                close(fd_output);
        return 0;

}