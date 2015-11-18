#include <leveldb/c.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int MIN_HASH;

/*@description:Function to write key value pair to leveldb
Input:
        char *key : Segment id
        char *value : minhash
Output:
        int ret           : -1 on failure and 0 on success
*/
int write_to_db(char *key, char *value);

/*@description:Function to read value pair form leveldb
Input:
        char *key : Segment id
Output:
        int ret           : -1 on failure and 0 on success
*/
int read_from_db(char *key, MIN_HASH min_hash[20], leveldb_t *db);

/*@description:Function to delete key value pair from leveldb
Input:
        char *key : Segment id
Output:
        int ret           : -1 on failure and 0 on success
*/
int delete_from_db(char *key);

/*@description:Function to delete leveldb
Input:
        void
Output:
        int ret           : -1 on failure and 0 on success
*/
int destroy_db();

/*@description:Function to find similarity between segment.
Input:
        MIN_HASH min_hash[20] : Min_hash of a current segment.
        int per_of_similarity : Percentage of similarity between segments.
Output:
        int ret           : -1 on failure and 0 on success
*/
int similarity_of_minhash(MIN_HASH min_hash[20], int *per_of_similarity, char **high_similarity_seg, int *high_seg_len);

int last_seg();