#include "ydl_hash.h"

/*Function to create hash for a given block
Input:void
Output:int*/
int
ydl_hash::init_hash_store()
{

        int ret         =       -1;

        hash_fd = open("filehashDedup.txt", O_APPEND|O_CREAT|O_RDWR,
                S_IRUSR|S_IWUSR);
        if (hash_fd == -1) {
                printf("\nCreation of hash file failed with error [%s]\n",
                        strerror(errno));
                goto out;
        }
        ret = 0;

out:
        return ret;

}

/*Function to write contents to a hash file.
Input:char *buff,int offset
Output:int
*/
int
ydl_hash::insert_hash(char *buff, int offset)
{

        size_t          length;
        int ret         =       -1;

        length = strlen(buff);
        if (write (hash_fd, &length, INT_SIZE) == -1) {
                printf("\nWrite failed with error%s\n", strerror(errno));
                goto out;
        }
        if (-1 == write(hash_fd, buff, length)) {
                printf("\nWrite1 failed with error%s\n", strerror(errno));
                goto out;
        }
        if (write (hash_fd, &offset, INT_SIZE) == -1) {
                printf("\nWrite failed with error%s\n", strerror(errno));
                goto out;
        }
        ret = 0;
out:
        return ret;

}

/*Function to check whether a hash is present in hash store or not.
Input:char *out
Output:int
*/
int
ydl_hash::searchhash(char *out)
{

        struct stat             st;
        int     fd2             =               hash_fd;
        int     size            =               0;
        size_t  length          =               0;
        int     ret             =               -1;
        int     offset          =               0;
        char    *buffer         =               NULL;

        fstat(hash_fd, &st);
        size = st.st_size;
        /*Rewind the stream pointer to the start of temporary file*/
        if (-1 == lseek(hash_fd, 0, SEEK_SET)) {
                printf("\nLseek failed with error: [%s]\n", strerror(errno));
                goto out;
        }
        if (size == 0) {
                ret = 1;
                goto out;
        }
        while (size > 0) {
                ret = read(fd2, &length, INT_SIZE);
                if (ret == -1) {
                        printf("\nError while reading %s\n", strerror(errno));
                        goto out;
                }
                buffer = (char *)calloc(1, length+1);
                ret = read(fd2, buffer, length);
                if (ret == -1) {
                        printf("\nRead failed with error %s\n",
                                strerror(errno));
                        goto out;
                }
                ret = read(fd2, &offset, INT_SIZE);
                if (ret == -1) {
                        printf("\nError while reading %s\n", strerror(errno));
                        goto out;
                }

                buffer[length] = '\0';
                if (strcmp(out, buffer) == 0) {
                        ret = 0;
                        clean_buff(&buffer);
                        break;
                }
                size -= (length+INT_SIZE+INT_SIZE);
                clean_buff(&buffer);
                ret = 1;
        }
out:
        return ret;

}

/*Function to get the position of specific block in hash.
Input:char* hash
Output:int
*/
int
ydl_hash::getposition(char *hash)
{

        struct stat             st;
        int     size    =       0;
        size_t  length  =       0;
        int     ret     =       -1;
        int     offset  =       0;
        char    *buffer  =       NULL;
        int h_length    =       0;

        fstat(hash_fd, &st);
        size = st.st_size;
        h_length = strlen(hash);
        /* rewind the stream pointer to the start of temporary file*/
        if (-1 == lseek(hash_fd, 0, SEEK_SET)) {
                printf("\nLseek failed with error: [%s]\n", strerror(errno));
                goto out;
        }
        if (size == 0) {
                ret = 1;
                goto out;
        }

        while (size > 0) {
                ret = read(hash_fd, &length, INT_SIZE);
                if (ret == -1) {
                        printf("\nError while reading %s", strerror(errno));
                        goto out;
                }
                buffer = (char *)calloc(1, length+1);
                hash[h_length] = '\0';
                ret = read(hash_fd, buffer, length);
                if (ret == -1) {
                        printf("\nRead failed with error %s\n",
                                strerror(errno));
                        goto out;
                }
                ret = read(hash_fd, &offset, INT_SIZE);
                if (ret == -1) {
                        printf("\nError while reading %s", strerror(errno));
                        goto out;
                }
                buffer[length] = '\0';
                if (strcmp(hash, buffer) == 0) {
                        ret = 0;
                        clean_buff(&buffer);
                        break;
                }
                size -= (length+INT_SIZE+INT_SIZE);
                clean_buff(&buffer);
                ret = -1;
        }
out:
        if (ret == -1)
                return ret;
        else
                return offset;

}

/*Function to close hash fd.
Input:void
Output:int*/
int
ydl_hash::fini_hash_store()
{

        int ret         =       -1;

        if (hash_fd != -1)
                ret = close(hash_fd);
        if (ret == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ret = 0;

out:
        return ret;

}
