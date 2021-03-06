#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vector.h"
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
#if defined(CFLAG)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#include <linux/limits.h>
#define SHA1 CC_SHA1
#else
#include<openssl/ssl.h>
#include<openssl/sha.h>
#endif
#define int_size sizeof(int)

typedef unsigned char DIGEST;

/*@description:Function to get hash of a particular block.
@in: vector_ptr list-block contents strored in vector,int length-length of buffer,
        int hash_type-type of hash(sha1 or md5)
@out: char** hash-hash generated from a block,int *h_length-length of hash generated
@return: hash */
int get_hash(int hash_type,char** hash,int *h_length, vector_ptr list);

/*@description:Function to insert hash to hashstore
@in: char *buff-buffer that contains hash,int offset-starting position of block
@out: int 
@return: -1 for error and 0 if inserted successfully */
int insert_hash(char *buff,int offset);

/*@description:Function to insert block to blockstore
@in: vector_ptr list-buffer containing block,size_t length-size of block
@out: int 
@return: -1 for error and 0 if inserted successfully */
int insert_block(vector_ptr list,size_t length);

/*@description:Function to write the full path of file to catalog
@in: char* filename-filename of file that has been deduped
@out: int 
@return: -1 for error and 0 if inserted successfully */
int writecatalog(char* filename);

/*@description:Function to read the full path of file from catalog
@in: int fc-file descriptor of catalog file
@out: int 
@return: -1 for error and 0 if read successfully */
int readcatalog(int fc);

/*@description:Function to check whether path is present or not
@in: char out[]-path,int filedes-file descriptor of file
@out: int 
@return: -1 for error and 0 if found. */
int searchpath(char out[],int filedes);

/*@description:Function to check whether file exist or not.
@in: char *filename-filename of arguments that has been passed via CLI.
@out: int 
@return: -1 for error and 0 if found. */
int file_exist(char *filename);

/*@description:Function to write hash,beginning offset and ending offset of block to stub
@in: char *buff-buffer containing hash,size_t l-length of hash,int filedes-file
descriptor of stub,int b_offset-beginning offset of block,int e_offset-ending offset of block
@out: int 
@return: -1 for error and 0 if found. */
int write_to_stub(char buff[],size_t l,int filedes,int b_offset,int e_offset);

/*@description:Function to get specific block from specified position 
@in: int pos-position of block,
@out: char*
@return: block */
char* get_block(int pos, int *l);

/*@description:Function to check single instance of block of specified position
@in: int  st1-filedescriptor of stub,int b_offset-beginning offset of block,
int e_offset-ending offset of block
@out: int 
@return: -1 for error and 0 if found. */
int  searchstubhash(int  st1,int b_offset,int e_offset);

/*@description:Function to create hash using md5.
@in: vector_ptr list-Vector to store block for which the hash is generated,
int length-length of block
@out: char*-returns hash of block using md5
@return: hash */
DIGEST *str2md5(vector_ptr list);

/*@description:Function to create hash using sha1.
@in: vector_ptr list-Vector to store block for which the hash is generated,
int length-length of block
@out: char*-returns hash of block using sha1
@return: hash */
char* sha1(vector_ptr list);

/*@description:Function to insert block to blockstore object
@in: vector_ptr list-buffer containing block,size_t length-size of block
@out: int 
@return: -1 for error and 0 if inserted successfully */
int insert_block_to_object(char *hash,vector_ptr list, char *store_path);

/*@description:Function to get chunk from file
@in: char **buffer-buffer containing block,size_t length-size of block, int fd_input -
 file descriptor of file, int chunk_type - type of chunk to be used.
@out: int
@return: -1 for error and 0 if inserted successfully */
int get_next_chunk(int fd_input, int chunk_type,int block_size,
        char **buffer, int *length);

/*@description:Function to insert block to blockstore object
@in: vector_ptr list-buffer containing block,size_t length-size of block, char *hash-
hash value of chunk, int h_length - length of the hash, int store - type of store,
int e_offset - Ending offset, int b_offset - Beginning offset.
@out: int 
@return: -1 for error and 0 if inserted successfully */
int chunk_store(vector_ptr list, char *hash, int length, int h_length,
        int e_offset, int b_offset, int fd_stub, int store, char *store_path);

