#include "ydl_object_store.h"

/*Function to insert block to blockstore object
Input:  vector_ptr list : buffer containing block,size_t length-size of block
        char *hash      : hash value of the block
Output: int : -1 for error and 0 if inserted successfully */

int
ydl_object_store::insert_block_to_object(char *hash, vector_ptr list)
{

        DIR *dp1 = NULL;
        DIR *dp2 = NULL;
        DIR *dp3 = NULL;
        int ret = 0;
        int fd = -1;
        vector_ptr temp_node = NULL;
        char path[1024], filename[50];
        struct stat st;

        if (getcwd(path, sizeof(path)) == NULL) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        sprintf(path, "%s/block", path);
        dp1 = opendir(path);
        if (NULL == dp1) {
                ret = mkdir(path, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        sprintf(path, "%s/%c%c", path, hash[0], hash[1]);
        dp2 = opendir(path);
        if (NULL == dp2) {
                ret = mkdir(path, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }

        sprintf(path, "%s/%c%c", path, hash[2], hash[3]);
        dp3 = opendir(path);
        if (NULL == dp3) {
                ret = mkdir(path, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }

        sprintf(filename, "%s/%s.txt", path, hash);
        if (stat (filename, &st) != 0) {
                fd = open(filename, O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
                if (fd < 1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                do {
                        temp_node = list;
                        if (-1 == write(fd, temp_node->vector_element,
                                temp_node->length)) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                        }
                        list = list->next;
                        free(temp_node);
                } while (list != NULL);
        }
        ret = 0;
out:
        if (dp1 != NULL)
                closedir(dp1);
        if (dp2 != NULL)
                closedir(dp2);
        if (dp3 != NULL)
                closedir(dp3);
        if (fd != -1)
                close(fd);
        return ret;
}

/*Function to insert block to blockstore object
Input:  int *length     : length of the hash
        char *hash      : hash value of the block
Output: int : -1 for error and 0 if inserted successfully */
char*
ydl_object_store::get_block_from_object(char *hash, int *length)
{

        DIR *dp1 = NULL;
        DIR *dp2 = NULL;
        int ret = 0;
        int fd = -1;
        char path[1024], filename[1024];
        char *buffer = NULL;
        struct stat st;

        if (getcwd(path, sizeof(path)) == NULL) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        sprintf(path, "%s/block/%c%c", path, hash[0], hash[1]);
        dp1 = opendir(path);
        if (NULL == dp1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        sprintf(path, "%s/%c%c", path, hash[2], hash[3]);
        dp2 = opendir(path);
        if (NULL == dp2) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        sprintf(filename, "%s/%s.txt", path, hash);
        if (stat (filename, &st) == 0) {
                *length = st.st_size;
                fd = open(filename, O_RDONLY, S_IRUSR|S_IWUSR);
                if (fd < 1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                buffer = (char *)calloc(1, *length);
                ret = read(fd, buffer, *length);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }

out:
        if (dp1 != NULL)
                closedir(dp1);
        if (dp2 != NULL)
                closedir(dp2);
        if (fd != -1)
                close(fd);
        return buffer;
}
