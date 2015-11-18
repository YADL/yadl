#include "ldb.h"
#include "main.h"
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
        leveldb_put(db, write_opt, key, strlen(key), value, strlen(value),
                &ret_value);
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
        leveldb_t *db : Pointer to level db.
Output:
        int ret           : -1 on failure and 0 on success
*/
int read_from_db(char *key, MIN_HASH min_hash[20], leveldb_t *db)
{

        leveldb_readoptions_t *read_opt =       NULL;
        leveldb_options_t *option       =       NULL;
        char *ret_value                 =       NULL;
        char *read                      =       NULL;
        size_t read_len                 =          0;
        int ret                         =         -1;
        const char dlmtr[2]    =       ",";
        char *token = NULL;
        char *str = NULL;
        char    *saveptr       =       NULL;
        int i = 0;

        if(db == NULL) {
                option = leveldb_options_create();
                leveldb_options_set_create_if_missing(option, 1);
                db = leveldb_open(option, "db", &ret_value);
                if (ret_value != NULL) {
                        fprintf(stderr, "Error in opening\n");
                        goto out;
                }
                clean_buff(&ret_value);
        }
        read_opt = leveldb_readoptions_create();
        read = leveldb_get(db, read_opt, key, strlen(key), &read_len, &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Read fail.\n");
                goto out;
        }

        for (str = read ; ; str = NULL) {
                token = strtok_r(str, dlmtr, &saveptr);
                if (i == 20 || token == NULL)
                        break;
                min_hash[i] = atoi(token);
                i++;
        }

        clean_buff(&read);
        clean_buff(&ret_value);
        ret = 0;
out:
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

/*Function to find similarity between segment.
Input:
        MIN_HASH min_hash[20] : Min_hash of a current segment.
        int per_of_similarity : Percentage of similarity between segments.
Output:
        int ret           : -1 on failure and 0 on success
*/
int similarity_of_minhash(MIN_HASH min_hash[20], int *per_of_similarity,
char **high_similarity_seg, int *high_seg_len)
{
        float per;
        int union_count,intr_count,f=0;
        MIN_HASH pre_min_hash[20] = {0};
        leveldb_t *db                   =       NULL;
        leveldb_options_t *option       =       NULL;
        leveldb_readoptions_t *roptions =       NULL;
        leveldb_iterator_t *multi       =       NULL;
        char *ret_value                 =       NULL;
        int ret                         =         -1;
        int i                           =          0;
        int j                           =          0;
        int high_similarity             =         -1;
        const char *key;
        char *seg_key                   =       NULL;
        size_t key_len;

        option = leveldb_options_create();
        leveldb_options_set_create_if_missing(option, 1);
        db = leveldb_open(option, "db", &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Error in opening\n");
                goto out;
        }
        clean_buff(&ret_value);
        roptions = leveldb_readoptions_create();
        multi = leveldb_create_iterator(db,roptions);
        leveldb_iter_seek_to_first(multi);
        do {
                key = leveldb_iter_key(multi, &key_len);
                seg_key = (char*)calloc(1, key_len);
                memcpy(seg_key, key, key_len);
                ret = read_from_db(seg_key, pre_min_hash, db);
                if (ret != 0)
                        goto out;
                for(i = 0 ; i < 20; i++) {
                        for(j = 0; j < 20; j++) {
                                if(min_hash[i] == pre_min_hash[j]){
                                        f = 1;
                                        break;
                                }
                        }
                        if(f == 1){
                                union_count++;
                                intr_count++;
                        } else {
                                union_count++;
                        }
                        f=0;
                }
                per = ((float)intr_count/union_count)*100;
                if(per > high_similarity) {
                        high_similarity = per;
                        *high_similarity_seg = (char *)malloc(strlen(seg_key));
                        memcpy( *high_similarity_seg, seg_key, strlen(seg_key)+1);
                        *high_seg_len = strlen(seg_key);
                }
                intr_count = 0;
                union_count = 0;
                clean_buff(&seg_key);
                leveldb_iter_next(multi);
        }while(leveldb_iter_valid(multi));
        *per_of_similarity = high_similarity;
        ret = 0;
out:
        leveldb_iter_destroy(multi);
        leveldb_close(db);
        return ret;
}

int last_seg() {

        leveldb_t *db                   =       NULL;
        leveldb_options_t *option       =       NULL;
        leveldb_readoptions_t *roptions =       NULL;
        char *read                      =       NULL;
        size_t read_len                 =          0;
        char *ret_value                 =       NULL;
        int ret                         =         -1;

        if( opendir("db") == NULL )
                goto out;
        option = leveldb_options_create();
        leveldb_options_set_create_if_missing(option, 1);
        db = leveldb_open(option, "db", &ret_value);
        if (ret_value != NULL) {
                goto out;
        }
        clean_buff(&ret_value);
        roptions = leveldb_readoptions_create();
        read = leveldb_get(db, roptions, "Count", 5, &read_len, &ret_value);
        if (ret_value != NULL) {
                goto out;
        }
        leveldb_close(db);
        ret = atoi(read);

out:
        return ret;
}