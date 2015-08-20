#include "stub.h"
#include "clean_buff.h"

/*
 * Function to write contents to a stub file.
 * Input:char buff[],size_t l,int fd_stub,int b_offset,int e_offset
 * Output:int
 *
 */

int
init_stub_store(char *path, char *filename, int *fd_stub)
{

        int ret         =       -1;
        DIR *dp         =     NULL;
        char stub_path[1024];

        strcpy(stub_path,path);
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
write_to_stub(char *seg_id, DIGEST *digest, int hash_length, int fd_stub)
{

        int ret         =        -1;
        int seg_length  =        -1;

        seg_length = strlen(seg_id);
        if (write (fd_stub, &seg_length, int_size) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (write (fd_stub, seg_id, seg_length) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (write(fd_stub, &hash_length, int_size) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (write(fd_stub, digest, hash_length) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ret=0;
out:
        return ret;

}
