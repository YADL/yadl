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
        
        if( *buffer != NULL )
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
                return;
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
        ssize_t remaining_content_count =       0;
        ssize_t size                    =       0;
        ssize_t chunk_size              =       0;
        ssize_t buffer_length           =       0;

        struct stat st;

        fd = open (filename,O_RDONLY);
        if (fd < 0) 
        {
                fprintf (stderr, "Cannot open file\n");
                goto out;
        }

        fstat(fd, &st);
        size = st.st_size;
        while (size > 0) 
        {
                if(size < buffer_length)
                        buffer_length = size;
                else
                        buffer_length = BUFFER_LEN;

                buffer = (char*)calloc(1,buffer_length+1);
                if(buffer == NULL)
                {
                        fprintf (stderr, "Error in buffer allocation\n");
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
                remaining_content_incr  =       0;

                if( remaining_length > 0 && remaining_content_incr < N )
                {
                        end                     =       1;
                        remaining_content_incr  =       1;

                        if(remaining_length<=N)
                                remaining_content_count = remaining_length;
                        else
                                remaining_content_count = 1;
                }

                temp_buffer = (char*)calloc(1,N+1);
                if(temp_buffer == NULL)
                {
                        fprintf (stderr, "Error in buffer allocation\n");
                        ret     =-1;
                        goto out;
                }

                while(end < buffer_length)
                {
                        if( remaining_length > 0 && remaining_content_incr < N 
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

                                memcpy(temp_buffer + (N-remaining_content_incr), 
                                buffer + wstart, remaining_content_count);
                                remaining_content_incr++;
                        }
                        else
                        {
                                memcpy (temp_buffer, buffer + wstart, N);
                        }

                        temp_buffer[N+1] = '\0';
                        hash = calc_hash (temp_buffer, N, &ret);
                        if (ret == -1) 
                        {
                                fprintf (stderr, 
                                "Error calculating rolling hash!! \n" );
                                goto out;
                        }

                        memset(temp_buffer,0,sizeof(temp_buffer));

                        if ( (hash & FINGER_PRINT) == 0 ) 
                        {
                                if( remaining_content_incr != 0 )
                                {
                                        chunk_size = N + slide_incr + 
                                        remaining_length;
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
                                        buffer, end);

                                        remaining_content_count = 0;
                                        remaining_content_incr  = 0;

                                        clean_buff(&remaining_buffer_content);
                                        clean_buff(&remaining_window_content);
                                }
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

                                printf("Chunk\n%s\n\n",chunk_buffer);

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

                clean_buff(&temp_buffer);

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

                if(size==0 && remaining_length > 0)
                {
                        printf("Chunk\n%s\n\n",remaining_buffer_content);
                }
                if(size > buffer_length)
                        clean_buff(&buffer);
        }

        clean_buff(&buffer);
        ret = 0;

out:
        if (fd != -1)
                close(fd);

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
