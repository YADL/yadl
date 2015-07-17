#include "ydl_stub.h"

/*
 * Function to write contents to a stub file.
 * Input:char buff[],size_t l,int fd_stub,int b_offset,int e_offset
 * Output:int
 *
 */
int
ydl_stub::write_to_stub( char buff[], size_t length, int fd_stub,
                         int b_offset, int e_offset )
{

        int ret         =        -1;

        if (write (fd_stub, &length, INT_SIZE) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (-1 == write(fd_stub, buff, length)) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (write (fd_stub, &b_offset, INT_SIZE) == -1) {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        if (write (fd_stub, &e_offset, INT_SIZE) == -1) {
                fprintf(stderr,"%s\n",strerror(errno));
                goto out;
        }
        ret=0;
out:
        return ret;

}