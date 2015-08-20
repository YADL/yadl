#include <leveldb/c.h>
#include <stdio.h>
#include <string.h>

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
int read_from_db(char *key);

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