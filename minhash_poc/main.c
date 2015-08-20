#include "main.h"
#include "md5.h"
#include "sha1.h"
#include "Rabin_Karp.h"
#include "object_store.h"
#include "clean_buff.h"
#include "parsing.h"
#include "stub.h"
#include "ldb.h"

/*Function to generate minhash
Input:
        DIGEST *digest    : Digest of chunk
        int hash_length   : Length of the hash
        int seg_length    : Number of chunks per segment
        int chunk_count   : Number of chunks
        int size          : Size of the file
        int count         : keeps track of segment id
Output:
        int ret           : -1 on failure and 0 on success
*/
int cal_minhash (DIGEST *digest, int hash_length, int seg_length,
int chunk_count, int size, int count)
{

        int             i               =       0;
        int             index           =       0;
        int             index2          =       0;
        int             flag            =       0;
        int             minhash_fd      =       0;
        char            *path           =    NULL;
        static MIN_HASH min_hash[20]    =     {0};
        static int      j               =       0;
        MIN_HASH        int_value       =       0;
        char            value[200]      =      "";
        char            key[50]         =      "";

        path = getcwd(path, 1024);
        sprintf(path, "%s/minhash.txt", path);
        minhash_fd = open(path, O_CREAT|O_RDWR|O_APPEND, S_IWUSR|S_IRUSR);
        for (i = 0; i < hash_length; i = i + sizeof(int)) {
                int_value = int_value + *(digest + i);
                int_value << 8;
                int_value = int_value + *(digest + (i + 1));
                int_value << 8;
                int_value = int_value + *(digest + (i + 2));
                int_value << 8;
                int_value = int_value + *(digest + (i + 3));
                for (index = j; index > 0; index--) {
                        if (int_value > min_hash[index - 1]) {
                                break;
                        } else if (int_value == min_hash[index - 1]) {
                                flag = 1;
                                break;
                        }
                }
                if (flag == 0) {
                        for (index2 = j; index2 > index; index2--)
                                min_hash[index2] = min_hash[index2 - 1];
                        if (j < 20)
                                j++;
                        min_hash[index] = int_value;
                }
                flag = 0;
        }

        if (write (minhash_fd, &seg_length, int_size) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (chunk_count == seg_length || size == 0) {
                sprintf(key, "segment_%d", count);
                for(i = 0; i < 20; i++) {
                        sprintf(value, "%s%d", value,min_hash[i]);
                        min_hash[i] = 0;
                }
                printf("%s*\n", value);
                value[strlen(value)] = '\0';
                write_to_db(key, value);
                read_from_db(key);
                j = 0;
                printf("\n");
        }

out:
        return 0;

}

/*Function to insert chunks to segments.
Input:
        vector_ptr list   : Buffer containing block
        DIGEST *digest    : Digest of chunk
        int hash_length   : Length of the hash
        int seg_length    : Number of chunks per segment
        int *chunk_count  : Number of chunks
        int size          : Size of the file
        int *count        : keeps track of segment id
        int fd_stub       : File discriptor of stub
Output:
        int ret           : -1 on failure and 0 on success
*/
int insert_into_segment(vector_ptr list, int *chunk_count, DIGEST *digest,
int hash_length, int seg_length, int *count, int size, int fd_stub)
{

        char *path              =       NULL;
        char *hash              =       NULL;
        char seg_id[200]        =         "";
        int ret                 =         -1;
        DIR *dp                 =       NULL;
        vector_ptr temp_node    =       NULL;

        path = getcwd(path, 1024);
        sprintf(path, "%s/block/segment_%d", path, *count);
        if (*chunk_count == 0) {
                dp = opendir(path);
                if (NULL == dp) {
                        ret = mkdir(path, 0777);
                        if (ret < 0) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                        }
                }
        }

        hash = parse(digest, hash_length);
        if(hash == NULL) {
                ret = -1;
                goto out;
        }

        ret = insert_block_to_object(hash, list, path);
        if (ret < 0) {
                goto out;
        }

        sprintf(seg_id, "segment_%d", *count);
        ret = write_to_stub(seg_id, digest, hash_length, fd_stub);
        if (ret < 0) {
                goto out;
        }

        *chunk_count = *chunk_count + 1;
        ret = cal_minhash (digest, hash_length, seg_length, *chunk_count, size,
                *count);
        if (ret < 0) {
                goto out;
        }

        if (*chunk_count == seg_length || size == 0) {
                *chunk_count = 0;
                *count = *count + 1;
        }
        clean_buff(&path);
        ret = 0;
out:
        return ret;
}

int main(void)
{

        int ret         =       -1;
        int len         =        0;
        int length      =        0;
        int size        =        0;
        int fd_input    =       -1;
        int fd_stub     =       -1;
        int chunk_count =        0;
        int seg_length  =        0;
        int hash_length =        0;
        int chunk_flag  =        0;
        int chunk_length=        0;
        int count       =        0;
        char path[200]  =       "";
        char *chunk_buffer =  NULL;
        char *filename  =     NULL;
        char *sd_path   =     NULL;
        vector_ptr list =     NULL;
        DIGEST *digest  =     NULL;
        struct stat st;

        printf("Enter the full path of a file\n");
        if (scanf("%s",path) <= 0) {
                fprintf(stderr, "Error in getting path\n");
                goto out;
        }
        printf("Enter the number of chunk to be stored in a segment\n");
        if (scanf("%d", &seg_length) <= 0) {
                fprintf(stderr, "Error in getting length\n");
                goto out;
        }
        fd_input = open (path, O_RDONLY, S_IRUSR|S_IWUSR);
        if(fd_input < 0) {
                fprintf(stderr, "error in opening\n");
                goto out;
        } else {
                filename = basename(path);
                sd_path = dirname(path);
                ret = init_stub_store(sd_path, filename, &fd_stub);
                if (ret == -1) {
                        fprintf (stderr, "Error stub init\n");
                        goto out;
                }
                fstat(fd_input, &st);
                size = st.st_size;
                while(1) {
                        chunk_flag = 0;
                        list = NULL;
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
                        digest = str2md5(list);
                        hash_length = MD5_DIGEST_LENGTH;
                        ret = insert_into_segment(list, &chunk_count, digest,
                        hash_length, seg_length, &count, size, fd_stub);
                        clean_buff(&chunk_buffer);
                        length = 0;
                        if (size == 0)
                                break;
                }
        }
        ret = 0;
out:
        if(fd_stub =! -1)
                close(fd_stub);
        if(fd_input =! -1)
                close(fd_input);
        return ret;

}