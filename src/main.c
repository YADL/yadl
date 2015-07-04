#include "main.h"
#include "catalog.h"
#include "block.h"
#include "hash.h"
#include "dedup.h"
#include "restore.h"
#include "delete.h"
#include "string.h"

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

        filename = (char *)calloc(1, FILE_SIZE);
        ret = init_block_store();
        if (ret == -1)
                goto out;
        ret = init_hash_store();
        if (ret == -1)
                goto out;
        ret = init_catalog_store();
        if (ret == -1)
                goto out;

        while (1) {
                printf("\n1.Do you want to dedup a file\n");
                printf("2.Do you want restore a file\n");
                printf("3.delete a file\n");
                printf("4.Exit\n");
                printf("Enter your choice\n");
                ret = scanf("%d", &ch);
                if (ret <= 0)
                        goto out;

                switch (ch) {
                case 1:
                        printf("\nPlease give filename with the full path\n");
                        ret = scanf("%s", filename);
                        if (ret <= 0)
                                goto out;
                        ret = file_exist(filename);
                        if (ret == 0) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                continue;
                        }
                        printf("\nDo you want to do fixed or variable chunking\n");
                        printf("\n1.fixed or 2.variable\n");
                        while (scanf("%d", &chunk_type)) {
                                if (!(chunk_type == 1 || chunk_type == 2)) {
                                        printf("\nInvalid choice please enter valid choice\n");
                                } else {
                                        break;
                                }
                        }
                        printf("\nEnter block size\n");
                        printf("For variable chunking block size should be zero\n");
                        ret = scanf("%d", &block_size);
                        if (ret <= 0)
                                goto out;
                        printf("\nChoose algorithm to hash\n");
                        printf("1.md5 2.sha1\n");
                        while (scanf("%d", &hash_type)) {
                                if (!(hash_type == 1 || hash_type == 2)) {
                                        printf("\nInvalid choice please enter valid choice\n");
                                } else {
                                        break;
                                }
                        }
                        printf("\nSelect the store type\n");
                        printf("0 : default_store \n1 : object_store\n");
                        ret = scanf("%d", &store);
                        if (ret <= 0)
                                goto out;
                        switch (store) {
                        case 0:
                                printf("Default store selected\n");
                                store_type = default_store;
                                break;
                        case 1:
                                printf("Object store selected\n");
                                store_type = object_store;
                                break;
                        default:
                                printf("Invalid option\n");
                                goto out;
                        }
                        printf("Deduplication in progress...\n");
                        ret = dedup_file(filename, chunk_type, hash_type,
                                block_size, store_type);
                        if (ret == -1)
                                goto out;
                        break;
                case 2:
                        ret = restore_file();
                        if (ret == -1)
                                goto out;
                        break;
                case 3:
                        ret = delete_file();
                        if (ret == -1)
                                goto out;
                        break;
                case 4:
                        goto out;

                default:
                        printf("\nInvalid choice\nEnter valid choice\n ");
                }
        }
        ret = 0;
out:
        ret = fini_block_store();
        ret = fini_hash_store();
        ret = fini_catalog_store();
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
