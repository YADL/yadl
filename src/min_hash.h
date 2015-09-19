#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <getopt.h>
#include <leveldb/c.h>

typedef unsigned int MIN_HASH;

typedef struct vector *vector_ptr;

typedef unsigned char DIGEST;

struct struct_minhash {
        int no_of_prime;
        int seg_length;
        int per_of_similarity;
        char *minhash_type;
};
typedef struct struct_minhash minhash_config;


/*@description:Function to get chunk and minhash
Input:
        int seg_length    : Number of chunks per segment
        int per_of_similarity : Percentage of similarity between segments.
        char *path        : File path
Output:
        int ret           : -1 on failure and 0 on success
*/
int min_hash(char *path, minhash_config minhash_config_dtl);

/*@description:Function to generate minhash
Input:
        DIGEST *digest    : Digest of chunk
        int hash_length   : Length of the hash
        int seg_length    : Number of chunks per segment
        int chunk_count   : Number of chunks
        int size          : Size of the file
        int count         : keeps track of segment id
        int per_of_similarity : Percentage of similarity between segments.
Output:
        int ret           : -1 on failure and 0 on success
*/
int cal_minhash (DIGEST *digest, int hash_length, int seg_length,
int chunk_count, int size, int count, int per_of_similarity);

/*@description:Function to generate minhash
Input:
        vector_ptr list   : Buffer containing block
        DIGEST *digest    : Digest of chunk
        int hash_length   : Length of the hash
        int seg_length    : Number of chunks per segment
        int *chunk_count  : Number of chunks
        int size          : Size of the file
        int *count        : keeps track of segment id
        int fd_stub       : File discriptor of stub
        int per_of_similarity : Percentage of similarity between segments.
Output:
        int ret           : -1 on failure and 0 on success
*/
int insert_into_segment(vector_ptr list, int *chunk_count, DIGEST *digest,
int hash_length, int seg_length, int *count, int size, int per_of_similarity,
int no_of_prime, char *type);

/*@description:Function to generate minhash using XOR
Input:
        DIGEST *digest    : Digest of chunk
        int hash_length   : Length of the hash
        int seg_length    : Number of chunks per segment
        int chunk_count   : Number of chunks
        int size          : Size of the file
        int count         : keeps track of segment id
        int no_of_prime   : Range of prime.
Output:
        int ret           : -1 on failure and 0 on success
*/
int xor_minhash (DIGEST *digest, int hash_length, int seg_length,
int chunk_count, int size, int count, int no_of_prime, int per_of_similarity);