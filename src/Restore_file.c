#include "restore.h"
#include "catalog.h"
#include "clean_buff.h"
#include "stub.h"

/*Function to enter a filename that has to be restored.
Input:void
Output:int
*/
int
restore_file(char *file_path, char *store_path)
{

        int ret         =       -1;

        ret = comparepath(file_path);
        if (ret == -1) {
                goto out;
        }
        if (ret == 1) {
                printf("\nInvalid path");
                goto out;
        }
        ret = restorefile(file_path, store_path);
        if (ret == -1) {
                goto out;
        }
        ret = 0;
out:
        return ret;

}

/* Function to delete file and restore it with original contents.
Input   :  char* path
Output  :  int
*/
int
restorefile(char *path, char *store_path)
{

        int l                   =       0;
        int ret                 =      -1;
        int size                =       0;
        int size1               =       0;
        int     pos             =       0;
        char *buffer            =       NULL;
        char *buffer2           =       NULL;
        int length              =       0;
        int sd1                =       -1;
        struct stat             st;
        int bset                =       0;
        int eset                =       0;
        int fd2                =       -1;
        int store_type               =       -1;
        char *ts1               =       NULL;
        char *ts2               =       NULL;
        char *dir               =       NULL;
        char *filename1         =       NULL;

        ts1 = strdup(path);
        ts2 = strdup(path);
        dir = dirname(ts1);
        filename1 = basename(ts2);
        printf("%s\n", dir);
        printf("\npath : %s\n", path);
        printf("File : %s\n", filename1);
        ret = init_stub_store(store_path, filename1, &sd1);
        if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        } else {
                printf("\nStub file opened\n");
        }

        fstat(sd1, &st);
        size = st.st_size;
        fd2 = open(path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd2 < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        } else {
                printf("Restore file created\nRestore in progress...\n");
        }
        if (size > 0) {
                if (-1 == lseek(sd1,0,SEEK_SET)) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        if (size == 0) {
                printf("\nNo contents\n");
                ret = -1;
                goto out;
        }
        ret = read(sd1, &store_type, int_size);
        if (ret== -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        while (size > 0) {
                ret = read(sd1, &length, int_size);
                if (ret == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                buffer = (char *)calloc(1, length+1);
                ret = read(sd1, buffer, length);
                if (ret == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                ret = read(sd1, &bset, int_size);
                if (ret == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                ret = read(sd1, &eset, int_size);
                if (ret == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                buffer[length] = '\0';
                if (store_type == 0) {
                        pos = getposition(buffer);
                        if (pos == -1)
                                goto out;
                        buffer2 = get_block(pos, &l);
                        if (buffer2 == NULL) {
                                goto out;
                        }
                } else {
                        buffer2 = get_block_from_object(buffer, &l, store_path);
                        if (buffer2 == NULL)
                                goto out;
                }
                ret = write(fd2, buffer2, l);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                size1 -= (length+int_size+int_size+int_size);
                clean_buff(&buffer);
                clean_buff(&buffer2);
        }
        ret = 0;
out:
        return ret;

}
