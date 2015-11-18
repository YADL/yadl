#include <stdio.h>
#include <string.h>  
#include <stdlib.h> 
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h> 
#include <libgen.h> 
#include <error.h>
#define int_size sizeof(int)

/*@description:Function to write hash,beginning offset and ending offset of block to stub
@in: char *buff-buffer containing hash,size_t l-length of hash,int filedes-file descriptor of stub,int b_offset-beginning offset of block,int e_offset-ending offset of bloc
@out: int 
@return: -1 for error and 0 if found. */
int write_to_minhash_stub(void *buff, char *high_similarity_seg, int threshold_similarity, int similarity, int fd_stub, int buff_counter,  int high_seg_len,  char *store_path);

int init_minhash_stub(char *path, char *filename, int *fd_stub);

int delete_minhash_stub(char *path, char *filename);

/*Function to get the offset of block.
Input:char *hash and int fd_hash
Output:int : offset of block
*/
int get_minhash_offset(char *hash, int fd_hash);

/*Function to get the block from blockstore.
Input:int pos
Output:char* : Block
*/
char* get_minhash_block(int pos, int fd_block, int *l);
