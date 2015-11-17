#include "dedup.h"
#include "hash_db.h"
#include "clean_buff.h"
#include "main.h"
#include "Rabin_Karp.h"
#include "catalog.h"
#include "hash.h"
#include "namespace.h"
#include "stub.h"
#include "parsing.h"

#define NAME_SIZE 100

/*
Function to dedup a file whose path is specified by the user.
Input:char* filename,int chunk_type,int hash_type,int block_size
Output:int
*/
int
dedup_file (namespace_dtl namespace_input, char *file_path)
{

        char *filename          =       NULL;
        int chunk_type          =       0;
        int hash_type           =       0;
        int block_size          =       0;
        int store_type          =       0;
        int ret                 =       -1;
        int fd_input            =       -1;
        int fd_stub             =       -1;
        int length              =       0;
        int h_length            =       0;
        int b_offset            =       0;
        int e_offset            =       0;
        int size                =       0;
        int chunk_flag          =       0;
        int chunk_length        =       0;
        char confirm             =      -1;
        char *hash              =       NULL;
        char *ts1               =       NULL;
        char *filename1         =       NULL;
        char *buffer            =       NULL;
        char *chunk_buffer      =       NULL;
        FILE *fp                =       NULL;
        struct stat st;
        vector_ptr list         =       NULL;

        if (strcmp(namespace_input.hash_type, "md5") == 0)
                hash_type = 0;
        else
                hash_type = 1;

        if (strcmp(namespace_input.store_type, "default") == 0)
                store_type = 0;
        else
                store_type = 1;

        if (strcmp(namespace_input.chunk_scheme, "fixed") == 0) {
                chunk_type = 0;
                block_size = namespace_input.chunk_size;
        } else {
                chunk_type = 1;
                block_size = 0;
        }

        filename = file_path;
        ts1 = strdup(filename);
        filename1 = basename(ts1);
        fd_input = open(filename, O_RDONLY, S_IRUSR|S_IWUSR);
        if (fd_input < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ret = comparepath(filename);
        if (ret == -1) {
                goto out;
        }
        if (ret == 0) {
                printf("\nFile is already deduped."
                        "Do you want to overwrite?[Y/N]");
                if (scanf("%c", &confirm) <= 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                if (confirm != 'y' && confirm != 'Y')
                        goto out;
        }
        printf("\nDeduplication in progress...\n");
        ret = init_stub_store(namespace_input.store_path, filename1, &fd_stub);
        if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (write (fd_stub, &store_type, int_size) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        fstat(fd_input, &st);
        size = st.st_size;
        if (chunk_type == 0) {
                while (1) {
                        list = NULL;
                        if (size <= block_size) {
                                block_size = size;
                        }
                        b_offset = e_offset;
                        ret = get_next_chunk(fd_input, chunk_type, block_size,
                                &buffer, &length);
                        if (ret <= 0)
                                break;
                        list = insert_vector_element(buffer, list, &ret,
                                length);
                        if (ret == -1)
                                goto out;
                        e_offset += length-1;
                        size = size-length;
                        ret = get_hash(hash_type, &hash, &h_length, list);
                        if (ret == -1)
                                goto out;
                        ret = chunk_store(list, hash, length, h_length,
                        b_offset, e_offset, fd_stub, store_type,
                        namespace_input.store_path);
                        if (ret == -1)
                                goto out;
                        e_offset++;
                        if (size <= 0) {
                                ret = 0;
                                break;
                        }
                        clean_buff(&buffer);
                        clean_buff(&hash);
                }
        } else {
                fp      = fopen("./Rabin_Karp.csv", "w+");
                if (fp == NULL) {
                        fprintf(stderr,
                                "Error in creating file Rabin_Karp.csv\n");
                        goto out;
                }

                while (1) {
                        b_offset = e_offset;
                        list = NULL;
                        chunk_flag = 0;
                        while (chunk_flag == 0) {
                                chunk_buffer = get_variable_chunk(fd_input,
                                &ret, &size, &chunk_flag, &chunk_length);
                                if (ret == -1) {
                                        fprintf (stderr,
                                                "Error in variable chunking\n");
                                        goto out;
                                }
                                list = insert_vector_element(chunk_buffer, list,
                                        &ret, chunk_length);
                                length += chunk_length;
                                if (ret == -1)
                                        goto out;
                        }
                        e_offset += length - 1;
                        ret = get_hash(hash_type, &hash, &h_length, list);
                        if (ret == -1)
                                goto out;
                        ret = chunk_store(list, hash, length,
                                h_length, b_offset, e_offset, fd_stub,
                                store_type, namespace_input.store_path);
                        if (ret == -1)
                                goto out;
                        fprintf(fp, "%d\n", length);
                        e_offset++;
                        clean_buff(&chunk_buffer);
                        length = 0;
                        if (size == 0)
                                break;
                }
        }
        if (confirm == -1)
                ret = writecatalog(filename);
        if (ret == -1)
                goto out;
        ret = 0;
out:
        if (fp != NULL)
                fclose(fp);
        if (fd_input != -1)
                close(fd_input);
        return ret;
}

/*
Function to get next chunk from file.
Input:int fd_input,int chunk_type,int block_size,char** buffer,
int *length
Output:int
*/
int
get_next_chunk(int fd_input, int chunk_type, int block_size, char **buffer,
int *length)
{

        int ret         =       -1;

        *buffer = (char *)calloc(1, block_size+1);
        ret = read(fd_input, *buffer, block_size);
        if (ret < 1)
                goto out;
        *length = block_size;
out:
        return ret;

}

/*
Function to get hash from a specific algorithm.
Input:char *buffer,int length,int hash_type,char** hash,int *h_length
Output:int
*/
int
get_hash(int hash_type, char **hash, int *h_length, vector_ptr list)
{

        int ret         =       -1;
        char *buf       =       NULL;
        char *buff      =       NULL;
        DIGEST *digest  =     NULL;

        switch (hash_type) {
        case 0:
                digest = str2md5(list);
                buf = parse(digest, MD5_DIGEST_LENGTH);
                *hash = buf;
                *h_length = strlen(buf);
                break;
        case 1:
                buff = sha1(list);
                *hash = buff;
                *h_length = strlen(buff);
                break;
        }
        ret = 0;
        return ret;

}

/*
Function to store chunks in chunk store and hash in hash store.
Input:char *buff,char *hash,int length,int h_length,int e_offset,
int b_offset,int fd_stub
Output:int
*/
int
chunk_store(vector_ptr list, char *hash, int length, int h_length, int e_offset,
int b_offset, int fd_stub, int store_type, char *store_path)
{

        int off                 =       -1;
        int ret                 =       -1;

        if (store_type == 0) {
                ret = searchhash(hash);
                if (ret == -1) {
                        goto out;
                }
                if (ret == 0) {
                        ret = write_to_stub(hash, h_length, fd_stub, b_offset,
                                e_offset);
                        if (ret == -1) {
                                goto out;
                        }
                } else {
                        off = insert_block(list, length);
                        if (off == -1) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                        }
                        ret = insert_hash(hash, off);
                        if (ret == -1) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                        }
                        ret = write_to_stub(hash, h_length, fd_stub, b_offset,
                                e_offset);
                        if (ret == -1) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                        }
                }
        } else {
                ret = insert_block_to_object(hash, list, store_path);
                if (ret == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                ret = write_to_stub(hash, h_length, fd_stub, b_offset,
                        e_offset);
                if (ret == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        ret = 0;
out:
        return ret;

}