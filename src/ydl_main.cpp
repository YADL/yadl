#include "ydl_main.h"
#include "ydl_dedup.h"
#include "ydl_restore.h"
#include "ydl_delete.h"

/*Main program!*/
int
main (int argc, char *argv[])
{

        int chunk_type      =       0;
        int hash_type       =       0;
        char *filename      =       NULL;
        int ch              =       0;
        int block_size      =       0;
        int ret             =       -1;
        int store           =       0;
        enum stores store_type; 

        ydl_block       block_store;
        ydl_catalog     catalog_store;
        ydl_hash        hash_store;
        ydl_dedup       dedupe_obj;
        ydl_restore     restore_obj;
        ydl_delete      delete_obj;

        filename = (char *)calloc(1, FILE_SIZE);
        ret = block_store.init_block_store();
        if (ret == -1)
                goto out;
        ret = hash_store.init_hash_store();
        if (ret == -1)
                goto out;
        ret = catalog_store.init_catalog_store();
        if (ret == -1)
                goto out;

        while (1) {
                cout << "\n1.Do you want to dedup a file\n";
                cout << "2.Do you want restore a file\n";
                cout << "3.delete a file\n";
                cout << "4.Exit\n";
                cout << "Enter your choice\n";
                cin >> ch;
                switch (ch) {
                case 1:
                        cout << "\nPlease give filename with the "
                                << "full path\n";
                        cin >> filename;
                        ret = file_exist(filename);
                        if (ret == 0) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                continue;
                        }
                        cout << "\nDo you want to do fixed or "
                                << "variable chunking\n";
                        cout << "\n1.fixed or 2.variable\n";
                        while ( cin >> chunk_type ) {
                                if (!(chunk_type == 1 || chunk_type == 2)) {
                                        cout <<"\nInvalid choice please"
                                            << " enter valid choice\n";
                                } else {
                                        break;
                                }
                        }
                        cout << "\nEnter block size\n";
                        cout << "For variable chunking block size "
                                << "should be zero\n";
                        cin >> block_size;
                        cout << "\nChoose algorithm to hash\n" ;
                        cout << "1.md5 2.sha1\n";
                        while (cin >> hash_type) {
                                if (!(hash_type == 1 || hash_type == 2)) {
                                        cout <<"\nInvalid choice please"
                                              <<"enter valid choice\n";
                                } else {
                                        break;
                                }
                        }
                        cout << "\nSelect the store type\n";
                        cout << "0 : default_store\n1 : object_store\n";
                        cin >> store;
                        switch (store) {
                        case 0:
                                cout << "Default store selected\n";
                                store_type = default_store;
                                break;
                        case 1:
                                cout << "Object store selected\n";
                                store_type = object_store;
                                break;
                        default:
                                cout << "Invalid option\n";
                                goto out;
                        }
                        cout << "Deduplication in progress...\n";
                        ret = dedupe_obj.dedup_file(filename, chunk_type,
                                hash_type, block_size, store_type);
                        if (ret == -1)
                                goto out;
                        break;
                case 2:
                        ret = restore_obj.restore_file();
                        if (ret == -1)
                                goto out;
                        break;
                case 3:
                        ret = delete_obj.delete_file();
                        if (ret == -1)
                                goto out;
                        break;
                case 4:
                        goto out;

                default:
                        cout << "\nInvalid choice Enter valid choice\n";
                }
        }
        ret = 0;
out:
        ret = block_store.fini_block_store();
        ret = hash_store.fini_hash_store();
        ret = catalog_store.fini_catalog_store();
        return ret;

}

/*Function to check whether second argument exist in directory or not.
Input:char *filename
Output:int
*/
inline int
file_exist(char *filename)
{

        struct stat   buffer;

        return (stat (filename, &buffer) == 0);

}
