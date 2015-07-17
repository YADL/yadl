#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include<stdio.h>
#include<string.h>  
#include<stdlib.h> 
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<ctype.h> 
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<time.h> 
#include <libgen.h> 
#include<openssl/md5.h>
#include "clean_buff.h"
#include "ydl_rabin_karp.h"
#include "ydl_catalog.h"
#include "ydl_hash.h"
#include "ydl_object_store.h"
#include "ydl_stub.h"
#include "ydl_block.h"
#include "sha1.h"
#include "md5.h"

#if defined(CFLAG)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#include <linux/limits.h>
#define SHA1 CC_SHA1
#else
#include<openssl/ssl.h>
#include<openssl/sha.h>
#endif
#define INT_SIZE sizeof(int)
#define NAME_SIZE 100

class ydl_dedup
{
public:
        int dedup_file (char* filename,int chunk_type,int hash_type,
                        int block_size,int store);
        int get_hash(int hash_type,char** hash,int *h_length,
                     vector_ptr list);
        int get_next_chunk(int fd_input, int chunk_type,int block_size,
                           char **buffer, int *length);
        int chunk_store(vector_ptr list, char *hash, int length,
                        int h_length,int e_offset, int b_offset,
                        int fd_stub, int store);
};