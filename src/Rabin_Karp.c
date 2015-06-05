#include "Rabin_Karp.h"
#include "clean_buff.h"

/*Function to calculate the rolling hash for the first window of chunk.
Input:
        char* buffer            : Buffer to store window content
        y_uint32 *power         : Stores the value PRIME^window length
        int *ret                : To return the status 0 on success,
                                -1 on failure
Output:
        y_uint32 hash_value     : Rolling hash of a first window of chunk.
*/
y_uint32
calc_hash (char *buffer, y_uint32 *power, int *ret)
{

        int             i               =       0;
        y_uint32        hash_value      =       0;

        *power           =       1;

        if (buffer == NULL) {
                *ret    = -1;
                return    -1;
        }

        /*calculates the rolling hash for the first window of chunk and
         power's PRIME with window length*/
        for (i = 0; i < N; i++) {
                hash_value = (PRIME * hash_value + buffer[i]) % M;
                *power = (*power * PRIME) % M;
        }
        *ret = 0;
        return hash_value;

}

/*Function to keep track remaining content of previous buffer when there
 is no match with fingerprint

Input:
        ssize_t remaining_length       : Remaining length of the
                                        previous buffer
        char** remaining_buffer_content: Contains remaining content of
                                        the previous buffer
        char** remaining_window_content: Contains remaining content of
                                        the previous window
        char** buffer                  : Contains current buffer content
        ssize_t start                  : Starting offset of buffer
        ssize_t wstart                 : Starting offset of window
Output:
        int     ret                    : 0 on success, -1 on failure
*/
int
get_remaining_buffer_content(char **remaining_buffer_content,
        char **remaining_window_content, ssize_t remaining_length,
        char **buffer, ssize_t start, ssize_t wstart)
{

        int ret =       -1;

        *remaining_buffer_content = (char *)calloc(1,
                remaining_length);
        if (*remaining_buffer_content == NULL) {
                fprintf (stderr,
                        "Error in buffer allocation\n");
                goto out;
        }
        memcpy (*remaining_buffer_content, *buffer + start,
                remaining_length);

        /*remaining_window_content holds last window content of previous
          buffer, if remaining length of previous buffer is greater then
          window size*/
        if (remaining_length >= N) {
                *remaining_window_content = (char *)calloc
                        (1, N);
                if (*remaining_window_content == NULL) {
                        fprintf (stderr,
                                "Error in buffer allocation\n");
                        goto out;
                }

                memcpy (*remaining_window_content,
                        *buffer + wstart, N);
        }
        ret = 0;
out:
        return ret;

}

/*Function to get the chunk when there is a match with fingerprint
Input:
        ssize_t* remaining_length      : Remaining length of the
                                        previous buffer
        char** remaining_buffer_content: Contains remaining content of
                                        the previous buffer
        char** remaining_window_content: Contains remaining content of
                                        the previous window
        char** chunk_buffer            : Holds the chunk content
        char** buffer                  : Contains current buffer content
        ssize_t start                  : Starting offset of buffer
        ssize_t end                    : Ending offset of buffer
        ssize_t wstart                 : Starting offset of window
        ssize_t slide_incr             : Keeps track of buffer sliding
        ssize_t* remaining_content_incr: Keeps track of sliding of
                                        previous buffer
Output:
        int     ret                    : 0 on success, -1 on failure
*/
int
get_chunk_buffer(ssize_t *remaining_content_incr, ssize_t *remaining_length,
        char **chunk_buffer, char **buffer, char **remaining_buffer_content,
        char **remaining_window_content, ssize_t start, ssize_t end,
        ssize_t wstart, ssize_t slide_incr)
{

        ssize_t chunk_size      =       0;
        int     ret             =       -1;

        /*Generates the chunk by combining previous
        buffer content with current buffer content upto
        where the fingerprint is matched*/
        if (*remaining_content_incr != 0) {
                chunk_size = *remaining_length + end;
                *chunk_buffer = (char *)calloc
                        (1, chunk_size+1);
                if (*chunk_buffer == NULL) {
                        fprintf (stderr, "Error in buffer allocation\n");
                        goto out;
                }

                memcpy (*chunk_buffer, *remaining_buffer_content,
                        *remaining_length);
                memcpy (*chunk_buffer + *remaining_length, *buffer + start,
                        end);

                *remaining_content_incr  = 0;
                *remaining_length        = 0;
                clean_buff(remaining_window_content);
                clean_buff(remaining_buffer_content);
        }
        /*Generates the chunk from previous chunk
        boundary to where the fingerprint is matched*/
        else {
                chunk_size = N+slide_incr;
                *chunk_buffer = (char *)calloc
                        (1, chunk_size+1);
                if (*chunk_buffer == NULL) {
                        fprintf (stderr, "Error in buffer allocation\n");
                        goto out;
                }

                memcpy (*chunk_buffer,
                        *buffer + start, chunk_size);
        }
        ret = 0;
out:
        return ret;
}

/*Function to generate variable size chunk using rabin-karp.
Input:
        char* filename  : Name of the file to be chunked
Output:
        int             : 0 on success, -1 on failure
*/
char*
get_variable_chunk (int fd, int *ret, int *size)
{

        int     counter1                =       0;
        int     counter2                =       0;
        int     flag                    =       0;
        int     remaining_flag          =       0;
        static char *buffer;

        char    *temp_buffer              =       NULL;
        char    *chunk_buffer             =       NULL;
        char    *remaining_buffer_content =       NULL;
        char    *remaining_window_content =       NULL;

        y_uint32 hash    =       0;
        y_uint32 power   =       0;

        static ssize_t start;
        static ssize_t end;
        static ssize_t wstart;
        static ssize_t remaining_length;
        static ssize_t buffer_length;

        ssize_t remaining_content_incr  =       0;
        ssize_t slide_incr              =       0;

        while (*size > 0) {
                if (end == 0) {
                        if (*size < BUFFER_LEN)
                                buffer_length = *size;
                        else
                                buffer_length = BUFFER_LEN;

                        buffer = (char *)calloc(1, buffer_length);
                        if (buffer == NULL) {
                                fprintf (stderr,
                                "Error in buffer allocation\n");
                                *ret     = -1;
                                goto out;
                        }

                        *ret = read (fd, buffer, buffer_length);
                        if (*ret <= 0) {
                                fprintf (stderr, "Reading failed\n");
                                *ret     = -1;
                                goto out;
                        }

                        if (remaining_length == 0 && buffer_length <= N) {
                                *size = 0;
                                return buffer;
                        }

                        wstart                  =       0;
                        start                   =       0;
                        slide_incr              =       0;
                        end                     =       N;

                        /*If there is remaining content in previous buffer,
                        set the end pointer of new buffer and
                        remaining_content_incr according to the window size*/
                        if (remaining_length > 0
                        && remaining_content_incr < N) {
                                if (remaining_length < N) {
                                        end = N - remaining_length;
                                        remaining_content_incr  = N -
                                                remaining_length;
                                } else {
                                        end = 0;
                                        remaining_content_incr  = 1;
                                }
                        }
                }

                /*Loops until the end of the buffer, matches the hash with
                fingerprint. If match is successful then write chunks to
                file and length of chunk to .csv file*/
                while (end < buffer_length) {
                        if (flag == 0) {
                                temp_buffer = (char *)calloc(1, N);
                                if (temp_buffer == NULL) {
                                        fprintf (stderr,
                                                "Error in buffer allocation\n");
                                        *ret     = -1;
                                        goto out;
                                }

                                /*Creates the temp_buffer with previous buffers
                                remaining content and requried amount of data
                                from current buffer*/
                                if (remaining_length > 0
                                && remaining_content_incr < N
                                && remaining_content_incr != 0) {
                                        if (remaining_length < N) {
                                                memcpy (temp_buffer,
                                                   remaining_buffer_content,
                                                   remaining_length);

                                                wstart = end - 1;

                                                memcpy (temp_buffer + (N-end),
                                                        buffer, end);
                                                remaining_flag = 1;
                                        }
                                        remaining_content_incr++;
                                }
                                /*Creates the temp_buffer of window size
                                from current buffer*/
                                else {
                                        memcpy (temp_buffer,
                                                buffer + wstart, N);
                                }

                                hash = calc_hash (temp_buffer, &power, ret);
                                if (*ret == -1) {
                                        fprintf (stderr,
                                                "Error calculating rolling hash\n");
                                        goto out;
                                }
                                clean_buff(&temp_buffer);
                        }

                        if (hash == FINGER_PRINT) {
                                *ret = get_chunk_buffer
                                (&remaining_content_incr, &remaining_length,
                                &chunk_buffer, &buffer,
                                &remaining_buffer_content,
                                &remaining_window_content,
                                start, end, wstart, slide_incr);

                                if (*ret == -1)
                                        goto out;

                                slide_incr       = 0;
                                start            = end;
                                wstart           = end;
                                remaining_length = buffer_length - end;
                                end              += N;
                                return chunk_buffer;
                        } else {

                                /*Sliding window when there is remaning content
                                  in previous buffer with remaining length
                                  less then window size*/
                                if (remaining_content_incr != 0 &&
                                    remaining_content_incr < N &&
                                    remaining_flag == 1) {
                                        hash = (hash * PRIME - power *
                                        remaining_buffer_content[counter1] +
                                        buffer[end-1]) % M;

                                        remaining_content_incr++;
                                        counter1++;
                                } else {
                                        counter1 = 0;
                                }
                                /*Sliding window when there is remaning content
                                  in previous buffer with remaining length
                                  greater then window size*/
                                if (remaining_content_incr != 0 &&
                                    remaining_content_incr <= N &&
                                    remaining_flag == 0) {
                                        hash = (hash * PRIME - power *
                                        remaining_window_content[counter2] +
                                        buffer[end]) % M;

                                        remaining_content_incr++;
                                        counter2++;
                                } else {
                                        counter2 = 0;
                                        /*Sliding window with the current
                                          buffer*/
                                        if (remaining_content_incr == 0 ||
                                            remaining_content_incr >= N) {
                                                hash = (hash * PRIME - power *
                                                buffer[start+slide_incr] +
                                                buffer[end]) % M;

                                                slide_incr++;
                                        }
                                }

                                end++;
                                wstart++;
                                flag = 1;
                        }
                }
                /*Keeps track of remaining buffers content which is not
                 matched with fingerprint*/
                if (remaining_length > 0) {
                        *ret = get_remaining_buffer_content
                                (&remaining_buffer_content,
                                &remaining_window_content, remaining_length,
                                &buffer, start, wstart);
                        if (*ret == -1)
                                goto out;
                }

                end = 0;
                *size -= buffer_length;
                clean_buff(&buffer);
                if (*size == 0) {
                        start           = 0;
                        wstart          = 0;
                        buffer_length   = 0;
                }
                /*If buffer content is not matched with fingerprint and it has
                 reached end of file, consider remaining buffer content as
                 chunk*/
                if (*size == 0 && remaining_length > 0) {
                        if (remaining_length >= N)
                                clean_buff(&remaining_window_content);
                        remaining_length = 0;
                        return remaining_buffer_content;
                }
        }
        *ret = 0;
out:
        return NULL;
}
