#include "ydl_restore.h"

/*Function to enter a filename that has to be restored.
Input:void
Output:int
*/
int
ydl_restore::restore_file()
{

        int ret         =       -1;
        char* path      =       NULL;

        ydl_catalog catalog;

        cout << "\nDeduped files\n";
        ret = catalog.readfilecatalog();
        if (ret == -1) {
                goto out;
        }
        path = (char *)calloc(1, FILE_SIZE);
        cout << "\nEnter the full path of dedup file to be restored\n";
        if ((cin >> path) <= 0) {
                goto out;
        }
        ret = catalog.comparepath(path);
        if (ret == -1) {
                goto out;
        }
        if (ret == 1) {
                cout << "\nInvalid path";
                goto out;
        }
        ret = restorefile(path);
        if (ret == -1) {
                goto out;
        }
        ret = 0;
out:
        clean_buff(&path);
        return ret;

}

/* Function to delete file and restore it with original contents.
Input   :  char* path
Output  :  int
*/
int
ydl_restore::restorefile(char *path)
{

        int l                   =       0;
        int ret                 =      -1;
        char temp_name[NAME_SIZE]    = "";
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

        ydl_block block_store;
        ydl_hash hash_store;
        ydl_object_store object_store;

        ts1 = strdup(path);
        ts2 = strdup(path);
        dir = dirname(ts1);
        filename1 = basename(ts2);
        sprintf(dir, "%s/", dir);
        sprintf(temp_name, "%sDedup_%s", dir, filename1);
        cout << dir << endl;
        cout << "\npath" << path;
        cout << filename1 << endl;
        cout << endl << temp_name << endl;
        sd1 = open(temp_name, O_RDONLY, S_IRUSR|S_IWUSR);
        if (sd1 < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        } else {
                cout << "\nStub file opened\n";
        }

        fstat(sd1, &st);
        size = st.st_size;
        fd2 = open(path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd2 < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        } else {
                cout << "\nRestore file created\n";
        }
        if (size > 0) {
                if (-1 == lseek(sd1,0,SEEK_SET)) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        if (size == 0) {
                cout << "\nNo contents\n";
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
                        pos = hash_store.getposition(buffer);
                        if (pos == -1)
                                goto out;
                        buffer2 = block_store.get_block(pos, &l);
                        if (buffer2 == NULL) {
                                goto out;
                        }
                } else {
                        buffer2 = object_store.get_block_from_object(buffer, &l);
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
