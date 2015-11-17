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
        int threshold_similarity;
        char *minhash_type;
};
typedef struct struct_minhash minhash_config;

typedef struct namespace_struct namespace_dtl;

/*@description:Function to get chunk and minhash
Input:
        int seg_length    : Number of chunks per segment
        int threshold_similarity : Percentage of similarity between segments.
        char *path        : File path
Output:
        int ret           : -1 on failure and 0 on success
*/
int min_hash(namespace_dtl namespace_input, char *path, minhash_config minhash_config_dtl);

/*@description:Function to generate minhash
Input:
        DIGEST *digest    : Digest of chunk
        int hash_length   : Length of the hash
        int seg_length    : Number of chunks per segment
        int chunk_count   : Number of chunks
        int size          : Size of the file
        int count         : keeps track of segment id
        int threshold_similarity : Percentage of similarity between segments.
Output:
        int ret           : -1 on failure and 0 on success
*/
int cal_minhash (DIGEST *digest, int hash_length, int seg_length,
int chunk_count, int size, int count, int threshold_similarity, namespace_dtl namespace_input, char *seg_name,
char **high_similarity_seg, int *similarity, int *high_seg_len);

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
        int threshold_similarity : Percentage of similarity between segments.
Output:
        int ret           : -1 on failure and 0 on success
*/
int insert_into_segment(vector_ptr list, int *chunk_count, DIGEST *digest,
int hash_length, int seg_length, int *count, int size, int threshold_similarity,
int no_of_prime, char *type, namespace_dtl namespace_input, int length,
char **high_similarity_seg, int *similarity, int *high_seg_len);

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
int chunk_count, int size, int count, int no_of_prime, int threshold_similarity, namespace_dtl namespace_input, char *seg_name, char **high_similarity_seg,int *similarity,int *high_seg_len);

/*@description: Function to dedup the file .
Input:
        namespace_dtl namespace_input : Takes of the information of namespace
        char *file_path : Path of the file to be deduped
Output:
        int : Return 0 on success -1 on failure.
*/
int dedup_file (namespace_dtl namespace_input,char *file_path);

int create_extended_seg( char *high_similarity_seg,char *seg_name,namespace_dtl namespace_input);

int search_hash(char *hash, int fd_hash);