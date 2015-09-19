#include "main.h"
#include "md5.h"
#include "Rabin_Karp.h"
#include "object_store.h"
#include "clean_buff.h"
#include "parsing.h"
#include "stub.h"
#include "ldb.h"
#include "namespace.h"

/*Function to generate minhash
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
int chunk_count, int size, int count, int per_of_similarity)
{

        int             i               =       0;
        int             index           =       0;
        int             index2          =       0;
        int             flag            =       0;
        static MIN_HASH min_hash[20]    =     {0};
        static int      j               =       0;
        MIN_HASH        int_value       =       0;
        char            value[200]      =      "";
        char            key[50]         =      "";
        int             ret             =      -1;

        for (i = 0; i < hash_length; i = i + sizeof(int)) {
                int_value = int_value + *(digest + i);
                //int_value << 8;
                int_value = int_value + *(digest + (i + 1));
                //int_value << 8;
                int_value = int_value + *(digest + (i + 2));
                //int_value << 8;
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

        if (chunk_count == seg_length || size == 0) {
                if (count > 0) {
                        ret = similarity_of_minhash(min_hash, per_of_similarity);
                        if (ret != 0)
                                goto out;
                }
                sprintf(key, "segment_%d", count);
                for(i = 0; i < 20; i++) {
                        sprintf(value, "%s%d,", value,min_hash[i]);
                        min_hash[i] = 0;
                }
                value[strlen(value)] = '\0';
                write_to_db(key, value);
                j = 0;
        }
        ret = 0;
out:
        return ret;

}

/*Function to generate minhash using XOR
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
int chunk_count, int size, int count, int no_of_prime, int per_of_similarity)
{

        int             i               =       0;
        int             index2          =       0;
        int             flag            =       0;
        int             ret             =      -1;
        static MIN_HASH min_hash[20]    =     {0};
        static int      j               =       0;
        MIN_HASH        int_value       =       0;
        char            value[200]      =      "";
        char            key[50]         =      "";
        int             prime[20]           =  {0};
        int             k               =       0;
        int             p_i             =       3;
        int             c               =       0;
        int             p_count         =       0;
        static int      p_size          =       0;
        int             sort_index      =       0;

        for ( p_count = 2 ; p_count <= 20 ;  ) {
                for ( c = 2 ; c <= p_i - 1 ; c++ ) {
                        if ( p_i%c == 0 )
                        break;
                }
                if ( c == p_i )
                {
                        prime[p_size] = p_i;
                        p_size++;
                        p_count++;
                }
                p_i++;
        }

        for(i = 0; i < 20; i++) {
                for(k = 0; k < hash_length; k++) {
                        int_value = int_value + (digest[k]^prime[i]);
                }
                for (sort_index = j; sort_index > 0; sort_index--) {
                        if (int_value > min_hash[sort_index - 1]) {
                                break;
                        } else if (int_value == min_hash[sort_index - 1]) {
                                flag = 1;
                                break;
                        }
                }
                if (flag == 0) {
                        for (index2 = j; index2 > sort_index; index2--)
                                min_hash[index2] = min_hash[index2 - 1];
                        if (j < 20)
                                j++;
                        min_hash[sort_index] = int_value;
                }
                flag = 0;
        }
        for(i=0;i<20;i++)
        {
                printf("\n");
                printf("%d",min_hash[i]);
        }

        if (chunk_count == seg_length || size == 0) {
                if (count > 0) {
                        ret = similarity_of_minhash(min_hash, per_of_similarity);
                        if (ret != 0)
                                goto out;
                }
                sprintf(key, "segment_%d", count);
                for(i = 0; i < 20; i++) {
                        sprintf(value, "%s%d,", value,min_hash[i]);
                        min_hash[i] = 0;
                }
                value[strlen(value)] = '\0';
                write_to_db(key, value);
                j = 0;
                printf("\n");
        }
        ret = 0;
out:
        return ret;

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
        int per_of_similarity : Percentage of similarity between segments.
Output:
        int ret           : -1 on failure and 0 on success
*/
int insert_into_segment(vector_ptr list, int *chunk_count, DIGEST *digest,
int hash_length, int seg_length, int *count, int size, int per_of_similarity,
int no_of_prime, char *type)
{

        char *path              =       NULL;
        char *hash              =       NULL;
        int ret                 =         -1;
        DIR *dp                 =       NULL;
        DIR *dp1                =       NULL;

        path = getcwd(path, 1024);
        sprintf(path, "%s/block", path);
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
        sprintf(path, "%s/segment_%d", path, *count);
        if (*chunk_count == 0) {
                dp1 = opendir(path);
                if (NULL == dp1) {
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

        *chunk_count = *chunk_count + 1;
        if(strcmp(type,"default") == 0) {
                ret = cal_minhash (digest, hash_length, seg_length, *chunk_count, size,
                        *count, per_of_similarity);
                if (ret < 0) {
                        goto out;
                }
        } else {
                ret = xor_minhash (digest, hash_length, seg_length, *chunk_count, size,
                *count, no_of_prime, per_of_similarity);
                if (ret < 0) {
                        goto out;
                }
        }

        if (*chunk_count == seg_length || size == 0) {
                *chunk_count = 0;
                *count = *count + 1;
        }
        clean_buff(&path);
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        if (dp1 != NULL)
                closedir(dp1);
        return ret;
}

/*Function to get chunk and minhash of segment
Input:
        int seg_length    : Number of chunks per segment
        int per_of_similarity : Percentage of similarity between segments.
        char *path        : File path
Output:
        int ret           : -1 on failure and 0 on success
*/
int min_hash(char *path, minhash_config minhash_config_dtl)
{

        int ret         =       -1;
        int length      =        0;
        int size        =        0;
        int fd_input    =       -1;
        int chunk_count =        0;
        int hash_length =        0;
        int chunk_flag  =        0;
        int chunk_length=        0;
        int count       =        0;
        int seg_length  =        0;
        int no_of_prime =        0;
        int per_of_similarity  = 0;
        char *type      =     NULL;
        char *chunk_buffer =  NULL;
        vector_ptr list =     NULL;
        DIGEST *digest  =     NULL;
        struct stat st;

        seg_length      =        minhash_config_dtl.seg_length;
        no_of_prime     =        minhash_config_dtl.no_of_prime;
        per_of_similarity = minhash_config_dtl.per_of_similarity;
        type            =     minhash_config_dtl.minhash_type;

        fd_input = open (path, O_RDONLY, S_IRUSR|S_IWUSR);
        if(fd_input < 0) {
                fprintf(stderr, "Error in opening input file\n");
                goto out;
        } else {
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
                        hash_length, seg_length, &count, size, per_of_similarity,
                        no_of_prime, type);
                        clean_buff(&chunk_buffer);
                        length = 0;
                        if (size == 0)
                                break;
                }
        }
        ret = 0;
out:
        close(fd_input);
        return ret;

}
