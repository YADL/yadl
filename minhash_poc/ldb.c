#include "ldb.h"
#include "clean_buff.h"

/*Function to write key value pair to leveldb
Input:
        char *key : Segment id
        char *value : minhash
Output:
        int ret           : -1 on failure and 0 on success
*/
int write_to_db(char *key, char *value)
{

        leveldb_t *db                   =       NULL;
        leveldb_options_t *option       =       NULL;
        leveldb_writeoptions_t *write_opt =     NULL;
        char *ret_value                 =       NULL;
        int ret                         =         -1;

        option = leveldb_options_create();
        leveldb_options_set_create_if_missing(option, 1);
        db = leveldb_open(option, "db", &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Error in opening\n");
                goto out;
        }
        clean_buff(&ret_value);
        write_opt = leveldb_writeoptions_create();
        leveldb_put(db, write_opt, key, strlen(key), value, strlen(value), &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Write fail.\n");
                goto out;
        }
        clean_buff(&ret_value);
        ret = 0;
out:
        leveldb_close(db);
        return ret;

}

/*Function to read value pair form leveldb
Input:
        char *key : Segment id
        char *value : minhash
Output:
        int ret           : -1 on failure and 0 on success
*/
int read_from_db(char *key)
{

        leveldb_t *db                   =       NULL;
        leveldb_options_t *option       =       NULL;
        leveldb_readoptions_t *read_opt =       NULL;
        char *ret_value                 =       NULL;
        char *read                      =       NULL;
        size_t read_len                 =          0;
        int ret                         =         -1;

        option = leveldb_options_create();
        leveldb_options_set_create_if_missing(option, 1);
        db = leveldb_open(option, "db", &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Error in opening\n");
                goto out;
        }
        clean_buff(&ret_value);
        read_opt = leveldb_readoptions_create();
        read = leveldb_get(db, read_opt, key, strlen(key), &read_len, &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Read fail.\n");
                goto out;
        }
        printf("%s : %s\n", key, read);
        clean_buff(&read);
        clean_buff(&ret_value);
        ret = 0;
out:
        leveldb_close(db);
        return ret;

}

/*Function to delete key value pair from leveldb
Input:
        char *key : Segment id
Output:
        int ret           : -1 on failure and 0 on success
*/
int delete_from_db(char *key)
{

        leveldb_t *db;
        leveldb_options_t *option;
        leveldb_writeoptions_t *write_opt;
        char *ret_value        =      NULL;
        int ret         =       -1;

        option = leveldb_options_create();
        leveldb_options_set_create_if_missing(option, 1);
        db = leveldb_open(option, "db", &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Error in opening\n");
                goto out;
        }
        clean_buff(&ret_value);
        write_opt = leveldb_writeoptions_create();
        leveldb_delete(db, write_opt, key, strlen(key), &ret_value);
        if (ret_value != NULL) {
        fprintf(stderr, "Delete fail.\n");
        return(1);
        }
        clean_buff(&ret_value);
        ret = 0;
out:
        leveldb_close(db);
        return ret;

}

/*Function to delete leveldb
Input:
        void
Output:
        int ret           : -1 on failure and 0 on success
*/
int destroy_db()
{

        leveldb_options_t *option;
        char *ret_value        =      NULL;
        int ret         =       -1;

        option = leveldb_options_create();
        leveldb_options_set_create_if_missing(option, 1);
        leveldb_destroy_db(option, "db", &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Destroy fail.\n");
                goto out;
        }
        leveldb_free(ret_value);
        ret_value = NULL;
        ret = 0;
out:
        return ret;

}