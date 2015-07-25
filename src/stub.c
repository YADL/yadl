#include "stub.h"
#include "catalog.h"
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
delete_stub_store(char *path, char *filename)
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
write_to_stub(char buff[],size_t length,int fd_stub,int b_offset,int e_offset)
{

        int ret         =        -1;

        if (write (fd_stub, &length, int_size) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (-1 == write(fd_stub, buff, length)) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (write (fd_stub, &b_offset, int_size) == -1) {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if (write (fd_stub, &e_offset, int_size) == -1) {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=0;
out:
        return ret;

}
