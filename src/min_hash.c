#include "main.h"
#include "md5.h"
#include "Rabin_Karp.h"
#include "object_store.h"
#include "clean_buff.h"
#include "parsing.h"
#include "minhash_stub.h"
#include "ldb.h"
#include "namespace.h"
#include "catalog.h"

/*Function to generate minhash
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
int chunk_count, int size, int count, int threshold_similarity,
namespace_dtl namespace_input, char *seg_name, char **high_similarity_seg,
int *similarity, int *high_seg_len)
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
                        ret = similarity_of_minhash(min_hash, similarity,
                                high_similarity_seg, high_seg_len);
                        if (ret != 0)
                                goto out;
                }
                sprintf(key, "segment_%d", count);
                for(i = 0; i < 20; i++) {
                        sprintf(value, "%s%d,", value,min_hash[i]);
                        min_hash[i] = 0;
                }
                if(threshold_similarity > *similarity) {
                        value[strlen(value)] = '\0';
                        write_to_db(key, value);
                }
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
int chunk_count, int size, int count, int no_of_prime, int threshold_similarity,
namespace_dtl namespace_input, char *seg_name, char **high_similarity_seg,
int *similarity,int *high_seg_len)
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
        int      p_size          =       0;
        int             sort_index      =       0;

        for ( p_count = 2 ; p_count <= no_of_prime ;  ) {
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
        if (chunk_count == seg_length || size == 0) {
                if (count > 0) {
                        ret = similarity_of_minhash(min_hash, similarity,
                                high_similarity_seg, high_seg_len);
                        if (ret != 0)
                                goto out;
                }
                
                sprintf(key, "segment_%d", count);
                for(i = 0; i < 20; i++) {
                        sprintf(value, "%s%d,", value,min_hash[i]);
                        min_hash[i] = 0;
                }              
                if (threshold_similarity > *similarity) {
                        value[strlen(value)] = '\0';
                        write_to_db(key, value);
                }
                j = 0;
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
        int threshold_similarity : Percentage of similarity between segments.
Output:
        int ret           : -1 on failure and 0 on success
*/
int insert_into_segment(vector_ptr list, int *chunk_count, DIGEST *digest,
int hash_length, int seg_length, int *count, int size, int threshold_similarity,
int no_of_prime, char *type, namespace_dtl namespace_input, int length,
char **high_similarity_seg, int *similarity, int *high_seg_len)
{

        int ret                 =         -1;
        int fd_input            =         -1;
        int fd_hash             =         -1;
        int off                 =         -1;
        DIR *dp                 =       NULL;
        vector_ptr temp_node    =       NULL;
        char *hash              =       NULL;
        char store[1024];

        strcpy(store, namespace_input.store_path);
        sprintf(store, "%s/store_block/blocks", store);
        if (*chunk_count == 0) {
                dp = opendir(store);
                if (NULL == dp) {
                        ret = mkdir(store, 0777);
                        if (ret < 0) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                        }
                }
        }

        sprintf(store, "%s/segment_%d", store, *count);
        fd_input = open (store, O_CREAT|O_RDWR|O_APPEND, S_IWUSR|S_IRUSR);

        sprintf(store, "%s_hash", store);
        fd_hash = open (store, O_CREAT|O_RDWR|O_APPEND, S_IWUSR|S_IRUSR);
        hash = parse(digest, hash_length);
        if(hash == NULL) {
                ret = -1;
                goto out;
        }
        ret = search_hash(hash, fd_hash);
        if (ret == -1) {
                goto out;
        } else if (ret != 0) {
                if(fd_input < 0) {
                fprintf(stderr, "Error in creating segment file\n");
                goto out;
                } else {
                        if (length <= 0) {
                                goto out;
                        }
                        if (write (fd_input, &length, int_size) == -1) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                        }
                        off = lseek(fd_input, 1, SEEK_CUR);
                        if (off == -1)
                                goto out;
                        if (list == NULL) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                                
                        }

                        do {
                                temp_node = list;
                                if (-1 == write(fd_input, temp_node->vector_element,
                                        temp_node->length)) {
                                        fprintf(stderr, "%s\n", strerror(errno));
                                        goto out;
                                }
                                list = list->next;
                                free(temp_node);
                        } while (list != NULL);
                }
                if (fd_input)
                        close(fd_input);

                if(fd_hash < 0) {
                        fprintf(stderr, "Error in creating segment hash file\n");
                        goto out;
                } else {
                        hash = parse(digest, hash_length);
                        if(hash == NULL) {
                                ret = -1;
                                goto out;
                        }
                        length = strlen(hash);
                        if (write (fd_hash, &length, int_size) == -1) {
                                printf("\nWrite failed with error%s\n",
                                        strerror(errno));
                                goto out;
                        }
                        if (-1 == write(fd_hash, hash, length)) {
                                printf("\nWrite1 failed with error%s\n",
                                        strerror(errno));
                                goto out;
                        }
                        if (write (fd_hash, &off, int_size) == -1) {
                                printf("\nWrite failed with error%s\n",
                                        strerror(errno));
                                goto out;
                        }
                }
                if (fd_hash)
                        close(fd_hash);
        }

        clean_buff(&hash);

        *chunk_count = *chunk_count + 1;
        if(strcmp(type,"default") == 0) {
                ret = cal_minhash (digest, hash_length, seg_length, *chunk_count, size,
                        *count, threshold_similarity, namespace_input,
                        store, high_similarity_seg, similarity, high_seg_len);
                if (ret < 0) {
                        goto out;
                }
        } else {
                ret = xor_minhash (digest, hash_length, seg_length, *chunk_count, size,
                *count, no_of_prime, threshold_similarity, namespace_input, store,
                high_similarity_seg, similarity, high_seg_len);
                if (ret < 0) {
                        goto out;
                }
        }

        if (*chunk_count == seg_length) {
                *chunk_count = 0;
                *count = *count + 1;
        }
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        if (fd_input)
                close(fd_input);
        return ret;
}

/*Function to check whether a hash is present in hash store or not.
Input:char *out
Output:int
*/
int
search_hash(char *hash, int fd_hash)
{

        struct stat             st;
        int     fd2             =               fd_hash;
        int     size            =               0;
        size_t  length          =               0;
        int     ret             =               -1;
        int     offset          =               0;
        char    *buffer         =               NULL;

        fstat(fd_hash, &st);
        size = st.st_size;
        /*rewind the stream pointer to the start of temporary file*/
        if (-1 == lseek(fd_hash, 0, SEEK_SET)) {
                printf("\nLseek failed with error: [%s]\n", strerror(errno));
                goto out;
        }
        if (size == 0) {
                ret = 1;
                goto out;
        }
        while (size > 0) {
                ret = read(fd2, &length, int_size);
                if (ret == -1) {
                        printf("\nError while reading %s\n", strerror(errno));
                        goto out;
                }
                buffer = (char *)calloc(1, length+1);
                ret = read(fd2, buffer, length);
                if (ret == -1) {
                        printf("\nRead failed with error %s\n",
                                strerror(errno));
                        goto out;
                }
                ret = read(fd2, &offset, int_size);
                if (ret == -1) {
                        printf("\nError while reading %s\n", strerror(errno));
                        goto out;
                }

                buffer[length] = '\0';
                if (strcmp(hash, buffer) == 0) {
                        ret = 0;
                        clean_buff(&buffer);
                        break;
                }
                size -= (length+int_size+int_size);
                clean_buff(&buffer);
                ret = 1;
        }
out:
        return ret;

}



/*Function to get chunk and minhash of segment
Input:
        int seg_length    : Number of chunks per segment
        int threshold_similarity : Percentage of similarity between segments.
        char *path        : File path
Output:
        int ret           : -1 on failure and 0 on success
*/
int min_hash(namespace_dtl namespace_input, char *path,
minhash_config minhash_config_dtl)
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
        int fd_stub     =        0;
        int seg_length  =        0;
        int no_of_prime =        0;
        int threshold_similarity  = 0;
        int b_offset    =        0;
        int e_offset    =        0;
        int temp_len    =        0;
        int buff_counter=        0;
        int similarity  =        0;
        int high_seg_len = 0;
        char *ts1       =     NULL;
        char *filename  =     NULL;
        char *type      =     NULL;
        char *chunk_buffer =  NULL;
        char *hash      =     NULL;
        char *high_similarity_seg = NULL;
        char segment_id[1024];
        
        vector_ptr list =     NULL;
        DIGEST *digest  =     NULL;
        void *buffer    =     NULL;
        struct stat st;

        seg_length      =        minhash_config_dtl.seg_length;
        no_of_prime     =        minhash_config_dtl.no_of_prime;
        threshold_similarity = minhash_config_dtl.threshold_similarity;
        type            =     minhash_config_dtl.minhash_type;

        count = last_seg();
        if (count == -1) {
                count = 0;
        }
        ret = comparepath(path);
        if (ret == -1) {
                goto out;
        } else if(ret == 0) {
                printf("File already deduped\n");
                goto out;
        }
        
        ts1 = strdup(path);
        filename = basename(ts1);
        fd_input = open (path, O_RDONLY, S_IRUSR|S_IWUSR);
        if(fd_input < 0) {
                fprintf(stderr, "Error in opening input file\n");
                goto out;
        } else {
                printf("\nMinhash deduplication in progress...\n");
                ret = init_minhash_stub(namespace_input.store_path, filename, &fd_stub);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                fstat(fd_input, &st);
                size = st.st_size;
                while(1) {
                        chunk_flag = 0;
                        list = NULL;
                        b_offset = e_offset;
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
                        digest = str2md5(list);
                        
                        hash_length = MD5_DIGEST_LENGTH;

                        hash = parse(digest, hash_length);
                        if(hash == NULL) {
                                ret = -1;
                                goto out;
                        }
                        sprintf(segment_id,"segment_%d",count);
                        temp_len = strlen(segment_id);
                        if( chunk_count == 0 ) {
                                buff_counter = 0;
                                buffer = (void *)calloc(seg_length,
                                sizeof(int)+strlen(segment_id)+sizeof(int)+
                                hash_length+2*sizeof(int));
                        }
                        memcpy(buffer+buff_counter, &temp_len, sizeof(int));
                        buff_counter+= sizeof(int);
                        memcpy(buffer+buff_counter, segment_id, strlen(segment_id));
                        buff_counter+= strlen(segment_id);
                        temp_len = strlen(hash);
                        memcpy(buffer+buff_counter, &temp_len, sizeof(int));
                        buff_counter+= sizeof(int);
                        memcpy(buffer+buff_counter, hash, strlen(hash));
                        buff_counter+= strlen(hash);
                        memcpy(buffer+buff_counter, &b_offset, sizeof(int));
                        buff_counter+= sizeof(int);
                        memcpy(buffer+buff_counter, &e_offset, sizeof(int));
                        buff_counter+= sizeof(int);

                        ret = insert_into_segment(list, &chunk_count, digest,
                        hash_length, seg_length, &count, size, threshold_similarity,
                        no_of_prime, type, namespace_input, length,
                        &high_similarity_seg, &similarity, &high_seg_len);
                        if(chunk_count == 0 || size == 0) {
                                ret = write_to_minhash_stub(buffer,
                                high_similarity_seg, threshold_similarity,
                                similarity, fd_stub, buff_counter, high_seg_len,
                                namespace_input.store_path);
                                if (ret == -1)
                                        goto out;
                                if( high_similarity_seg != NULL )
                                        clean_buff(&high_similarity_seg);
                        }

                        length = 0;
                        e_offset++;
                        if (size == 0) {
                                break;
                        }
                }
        }
        sprintf(hash,"%d",count);
        write_to_db("Count",hash);
        ret = writecatalog(path);
        if (ret == -1)
                goto out;
        ret = 0;
out:
        close(fd_input);
        return ret;

}
