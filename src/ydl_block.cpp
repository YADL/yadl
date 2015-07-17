#include "ydl_block.h"

/*Function to create block file.
Input:void
Output:int*/
int ydl_block::init_block_store()
{
        int ret         =       -1;

        block_fd = open("blockstore.txt", O_APPEND|O_CREAT|O_RDWR,
                S_IWUSR|S_IRUSR);
        if (block_fd == -1) {
                cout << "\nCreation of block file failed with error "
                        << strerror(errno) << endl;
                goto out;
        }
        ret = 0;
out:
        return ret;

}

/*Function to write contents to a block file.
Input:vector_ptr list,size_t length
Output:int
*/
int ydl_block::insert_block(vector_ptr list,size_t length)
{
        int ret                 =       -1;
        vector_ptr temp_node    =       NULL;

        if (length <= 0) {
                goto out;
        }
        if (write (block_fd, &length, INT_SIZE) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ret = lseek(block_fd, 1, SEEK_CUR);
        ret = lseek(block_fd, 1, SEEK_CUR);
        if (ret == -1)
                goto out;
        if (list == NULL) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        do {
                temp_node = list;
                if (-1 == write(block_fd, temp_node->vector_element,
                        temp_node->length)) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                list = list->next;
                free(temp_node);
        } while (list != NULL);

out:
        return ret;

}

/*Function to get the block from blockstore.
Input:int pos
Output:char*
*/
char *ydl_block::get_block(int pos,int *l)
{
        struct stat                     st;
        int     size     =               0;
        int    length    =               0;
        int     ret      =               -1;
        char    *buffer   =               NULL;
        int     position =               1;

        fstat(block_fd, &st);
        size = st.st_size;
        /*rewind the stream pointer to the start of block file*/
        if (size > 0) {
                if (-1 == lseek(block_fd, 0, SEEK_SET)) {
                        cout << "\nLseek failed with error:"
                        << strerror(errno) << endl;
                        goto out;
                }
        }
        while (size > 0) {
                ret = read(block_fd, &length, INT_SIZE);
                *l = length;
                if (ret == -1) {
                        cout << "\nError while reading "
                                << strerror(errno) << endl;
                        goto out;
                }
                position = position+ret;
                if (length <= 0) {
                        goto out;
                }
                buffer = (char *)calloc(1, length+1);

                if (position == pos) {
                        ret = read(block_fd, buffer, length);
                        if (ret == -1) {
                                cout << "\nRead failed with error "
                                        << strerror(errno) << endl;
                                goto out;
                        }
                        ret = 0;
                        buffer[length] = '\0';
                        break;
                }
                ret = read(block_fd, buffer, length);
                if (ret == -1) {
                        cout << "\nRead failed with error "
                                << strerror(errno) << endl;
                        goto out;
                }
                position = position + length;
                buffer[length] = '\0';
                size -= (length+INT_SIZE);
                clean_buff(&buffer);
        }
        ret = 0;

out:
        if (ret == -1) {
                memset(buffer, 0, length+1);
        }
        return buffer;

}

/*Function to close block fd.
Input:void
Output:int*/
int ydl_block::fini_block_store()
{
        int ret         =       -1;

        if (block_fd != -1)
                ret = close(block_fd);
        if (ret == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ret = 0;
out:
        return ret;

}
