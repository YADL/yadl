#include "clean_buff.h"
#include "delete.h"

/*Function to delete file from a directory.
Input:void
Output:int
*/
int
delete_file(char *file_path, char *store_path)
{

        int fd_cat      =       -1;
        int ret         =       -1;
        int status      =       -1;
        char *filename  =       NULL;

        status = remove(filename);
        if (status == 0) {
                printf("\nFile deleted successfully.\n");
        } else {
                printf("\nUnable to delete the file\n");
                perror("\nError\n");
                goto out;
        }
        ret = 0;
out:
        clean_buff(&filename);
return ret;

}
