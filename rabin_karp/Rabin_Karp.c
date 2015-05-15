#include "Rabin_Karp.h"


/*Function to clean buffer contents.
Input:
        char** buffer   : Buffer to be cleaned
Output:
        void
*/
inline void 
clean_buff(char** buffer)
{
        if( *buffer != NULL && !buffer)
        {
                free(*buffer);
                *buffer = NULL;
        }
}

/*Function to check whether file is existing or not.
Input:
        char* filename  : Name of the file to be checked 
Output:
        int             : 0 on success, -1 on failure
*/
int 
file_exist (char *filename)
{
        struct stat   st;
        return (stat (filename, &st) == 0);
}

/*Function to find the rolling hash of a particular window.
Input:
        char* buffer            : Buffer to store window content
        ssize_t length          : Stores the length of the window
        int *ret                : To return the status 0 on success, 
                                -1 on failure
Output:
        y_uint32 hash_value     : Rolling hash of a particular window
*/
y_uint32
calc_hash (char *buffer, ssize_t length, int *ret)
{
        int             i               =       0;
        int             j               =       0;
        int             n               =       0;
        y_uint32        hash_value      =       0;
        y_uint32        power           =       0;

        if( buffer == NULL || length < 0 )
        {
                *ret    =-1;
                return   -1;
        }

        for(i=0;i<length;i++)
        {
                n       =       length - i;
                power   =       1;
                for(j=0;j<n;j++)
                        power *= PRIME;
                hash_value += (power * buffer[i]);
        }
        *ret = 0;
        return hash_value;
}

/*Function to generate variable size chunk using rabin-karp.
Input:
        char* filename  : Name of the file to be chunked 
Output:
        int             : 0 on success, -1 on failure
*/
int 
variable_chunking (char *filename) 
{
        int ret         =       -1;
        int fd          =       0;
        int fd_copy     =       0;
        FILE *fp        =       NULL;

        char*   buffer                  =       NULL;
        char*   temp_buffer             =       NULL;
        char*   chunk_buffer            =       NULL;
        char*   remaining_buffer_content=       NULL;
        char*   remaining_window_content=       NULL;

        y_uint32 hash   =       0;

        ssize_t start                   =       0;
        ssize_t end                     =       0;
        ssize_t wstart                  =       0;
        ssize_t slide_incr              =       0;
        ssize_t remaining_length        =       0;
        ssize_t remaining_content_incr  =       0;
        ssize_t size                    =       0;
        ssize_t chunk_size              =       0;
        ssize_t buffer_length           =       0;

        struct stat st;
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP|S_IROTH;

        fd      = open (filename,O_RDONLY);
        if (fd < 0) 
        {
                fprintf (stderr, "Cannot open file\n");
                goto out;
        }

        fd_copy     = open("./test_data_copy.txt",
        O_WRONLY | O_CREAT| O_RDONLY,mode);
        if (fd_copy < 0) 
        {
                fprintf (stderr, 
                "Error in creating file test_data_copy.txt\n");
                goto out;
        }

        fp      = fopen("./Rabin_Karp.csv","w+");
        if(fp==NULL)
        {
                printf("Error in creating file Rabin_Karp.csv\n");
                goto out;
        }

        fstat(fd, &st);
        size = st.st_size;
        while (size > 0) 
        {
                if(size < BUFFER_LEN)
                        buffer_length = size;
                else
                        buffer_length = BUFFER_LEN;

                buffer = (char*)calloc(1,buffer_length+1);
                if(buffer == NULL)
                {
                        fprintf (stderr,"Error in buffer allocation\n");
                        ret     =-1;
                        goto out;
                }

                ret = read (fd, buffer, buffer_length);
                if(ret < 0)
                {
                        fprintf (stderr, "Reading failed\n");
                        ret     =-1;
                        goto out;
                }
                buffer[buffer_length+1]='\0';

                wstart                  =       0;
                start                   =       0;
                slide_incr              =       0;
                end                     =       N;

                /*If there is remaining content in previous buffer, 
                set the end pointer of new buffer and 
                remaining_content_incr according to the window size*/
                if( remaining_length > 0 && remaining_content_incr < N )
                {
                        if(remaining_length<=N)
                        {
                                end = N - remaining_length;
                                remaining_content_incr  = N;
                        }
                        else
                        {
                                end = 1;
                                remaining_content_incr  = 1;
                        }
                }

                /*Loops until the end of the buffer, matches the hash 
                 with fingerprint. If match is successful then write 
                chunks to file and length of chunk to .csv file*/
                while(end < buffer_length)
                {
                        temp_buffer = (char*)calloc(1,N+1);
                        if(temp_buffer == NULL)
                        {
                                fprintf (stderr,
                                "Error in buffer allocation\n");
                                ret     =-1;
                                goto out;
                        }

                        /*Creates the temp_buffer with previous buffers 
                         remaining content and requried amount of data 
                         from current buffer*/
                        if( remaining_length > 0 && 
                        remaining_content_incr < N 
                        && remaining_content_incr != 0 )
                        {
                                if(remaining_length <= N)
                                        memcpy (temp_buffer, 
                                        remaining_buffer_content, 
                                        remaining_length);
                                else
                                        memcpy (temp_buffer, 
                                        remaining_window_content + 
                                        remaining_content_incr, 
                                        N - remaining_content_incr);

                                memcpy(temp_buffer + (N-end), 
                                buffer + wstart, end);
                                remaining_content_incr++;
                        }
                        /*Creates the temp_buffer of window size 
                         from current buffer*/
                        else
                        {
                                memcpy (temp_buffer,buffer + wstart, N);
                        }

                        temp_buffer[N+1] = '\0';
                        hash = calc_hash (temp_buffer, N, &ret);
                        if (ret == -1) 
                        {
                                fprintf (stderr, 
                                "Error calculating rolling hash!! \n" );
                                goto out;
                        }
                        
                        clean_buff(&temp_buffer);

                        if ( (hash & FINGER_PRINT) == 0 ) 
                        {
                                /*Generates the chunk by combining 
                                 previous buffer content with current 
                                buffer content upto where the 
                                fingerprint is matched*/
                                if( remaining_content_incr != 0 )
                                {
                                        chunk_size = remaining_length + 
                                        end;
                                        chunk_buffer = (char*)calloc
                                        (1,chunk_size+1);
                                        if(chunk_buffer == NULL)
                                        {
                                                fprintf (stderr, 
                                                "Error in buffer allocation\n");
                                                ret     =-1;
                                                goto out;
                                        }

                                        memcpy (chunk_buffer, 
                                        remaining_buffer_content, 
                                        remaining_length);
                                        memcpy (chunk_buffer + remaining_length, 
                                        buffer + start, end);

                                        remaining_content_incr  = 0;
                                        remaining_length        = 0;
                                        clean_buff(&remaining_window_content);
                                        clean_buff(&remaining_buffer_content);
                                }
                                /*Generates the chunk from previous chunk 
                                 boundary to where the fingerprint is matched*/
                                else
                                {
                                        chunk_size = N+slide_incr;
                                        chunk_buffer=(char*)calloc
                                        (1,chunk_size+1);
                                        if(chunk_buffer == NULL)
                                        {
                                                fprintf (stderr, 
                                                "Error in buffer allocation\n");
                                                ret     =-1;
                                                goto out;
                                        }

                                        memcpy (chunk_buffer, 
                                        buffer + start, chunk_size);
                                }

                                chunk_buffer[chunk_size+1]='\0';
                                ret = write(fd_copy,chunk_buffer,
                                strlen(chunk_buffer));
                                if(ret == -1)
                                {
                                    fprintf (stderr,
                                    "Error in writing chunks to file\n");
                                    goto out;
                                }
                                ret = fprintf(fp,"%lu,",strlen(chunk_buffer));
                                if(ret < 0)
                                {
                                    fprintf (stderr,
                                    "Error in writing chunks size to file\n");
                                    ret     =-1;
                                    goto out;
                                }

                                clean_buff(&chunk_buffer);

                                slide_incr      = 0;
                                start           = end;
                                wstart          = end;
                                remaining_length= buffer_length - end;
                                end             += N;

                        }
                        else
                        {
                                end++;
                                wstart++;
                                slide_incr++;
                        }
                }

                /*Keeps track of remaining buffers content which is not matched 
                 with fingerprint*/
                if(remaining_length > 0)
                {
                        remaining_buffer_content = (char*)calloc(1,
                        remaining_length+1);
                        if(remaining_buffer_content == NULL)
                        {
                                fprintf (stderr, 
                                "Error in buffer allocation\n");
                                ret     =-1;
                                goto out;
                        }
                        memcpy (remaining_buffer_content, buffer + start, 
                        remaining_length);
                        remaining_buffer_content[remaining_length+1]='\0';

                        if(remaining_length >= N)
                        {
                                remaining_window_content = (char*)calloc(1,N+1);
                                if(remaining_window_content == NULL)
                                {
                                        fprintf (stderr, 
                                        "Error in buffer allocation\n");
                                        ret     =-1;
                                        goto out;
                                }

                                memcpy (remaining_window_content, 
                                buffer + wstart, remaining_length);
                                remaining_window_content[N+1] = '\0';
                        }
                }

                size -= buffer_length;

                /*If buffer content is not matched with fingerprint and it has 
                 reached end of file, consider remaining buffer content as 
                 chunk*/
                if(size==0 && remaining_length > 0)
                {
                        ret = write(fd_copy,remaining_buffer_content,
                        strlen(remaining_buffer_content));
                        if(ret == -1)
                        {
                                fprintf (stderr,
                                "Error in writing chunks to file\n");
                                goto out;
                        }
                        ret = fprintf(fp,
                        "%lu",strlen(remaining_buffer_content));
                        if(ret == -1)
                        {
                                fprintf (stderr,
                                "Error in writing chunks to file\n");
                                ret     =-1;
                                goto out;
                        }
                        clean_buff(&remaining_buffer_content);
                }
                clean_buff(&buffer);
        }

        ret = 0;

out:
        if (fd != -1)
                close(fd);
        if (fd != -1)
                close(fd_copy);
        if ( fp != NULL)
                fclose(fp);

        return ret;
}

int main(int argc,char **argv)
{
        int ret         =       -1;
        char* filename  =       NULL;

        filename = argv[1];

        if(!file_exist (filename))
        {
                fprintf (stderr, "Error in opening file\n");
                goto out;
        }

        ret = variable_chunking(filename);
        if(ret == -1)
        {
                fprintf (stderr, "Error in variable chunking\n");
                goto out;
        }
        ret = 0;
out:
        return ret;
}
