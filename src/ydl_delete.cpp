#include "ydl_delete.h"

/*Function to delete file from a directory.
Input:void
Output:int
*/
int
ydl_delete::delete_file()
{

        int fd_cat      =       -1;
        int ret         =       -1;
        int status      =       -1;
        char *filename  =       NULL;

        filename = (char *)calloc(1, FILE_SIZE);
        fd_cat = open("filecatalog.txt", O_RDONLY);
        if (fd_cat < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        cout << "\n deduped files are\n";
        ret = readfilecatalog();
        if (ret == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        cout << "\nSelect the file you want to delete\n";
        while ( cin >> filename <= 0)
                ;
        cout << "\nFILE" << filename;
        status = remove(filename);
        if (status == 0) {
                cout << "\nFile deleted successfully.\n";
        } else {
                cout << "\nUnable to delete the file\n";
                perror("\nError\n");
                goto out;
        }
        ret = 0;
out:
        clean_buff(&filename);
return ret;

}
