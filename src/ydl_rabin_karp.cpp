#include "ydl_rabin_karp.h"

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
ydl_rabin_karp::calc_hash (char *buffer, y_uint32 *power, int *ret)
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
        for (i = 0; i < WINDOW_SIZE; i++) {
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
Output:
        int     ret                    : 0 on success, -1 on failure
*/
int
ydl_rabin_karp::get_remaining_buffer_content(char **remaining_buffer_content,
        char **remaining_window_content, ssize_t remaining_length,
        char **buffer, ssize_t start)
{

        int ret =       -1;

        *remaining_buffer_content = (char *)calloc(1,
                remaining_length + 1);
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
        if (remaining_length >= WINDOW_SIZE) {
                *remaining_window_content = (char *)calloc
                        (1, WINDOW_SIZE + 1);
                if (*remaining_window_content == NULL) {
                        fprintf (stderr,
                                "Error in buffer allocation\n");
                        goto out;
                }

                memcpy (*remaining_window_content,
                        *buffer + BUFFER_LEN - WINDOW_SIZE, WINDOW_SIZE);
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
        ssize_t slide_incr             : Keeps track of buffer sliding
        ssize_t* remaining_content_incr: Keeps track of sliding of
                                        previous buffer
Output:
        int     ret                    : 0 on success, -1 on failure
*/
int
ydl_rabin_karp::get_chunk_buffer(ssize_t *remaining_content_incr,
                                 ssize_t *remaining_length,
                                 char **chunk_buffer, char **buffer,
                                 char **remaining_buffer_content,
                                 char **remaining_window_content,
                                 ssize_t start, ssize_t end,
                                 ssize_t slide_incr, int *chunk_length)
{

        ssize_t chunk_size      =       0;
        int     ret             =      -1;

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
                memcpy (*chunk_buffer + *remaining_length, *buffer,
                        end);
                *chunk_length = *remaining_length + end;
                *remaining_content_incr  = 0;
                *remaining_length        = 0;
                clean_buff(remaining_window_content);
                clean_buff(remaining_buffer_content);
        }
        /*Generates the chunk from previous chunk
        boundary to where the fingerprint is matched*/
        else {
                *chunk_length = WINDOW_SIZE+slide_incr;
                *chunk_buffer = (char *)calloc
                        (1, *chunk_length+1);
                if (*chunk_buffer == NULL) {
                        fprintf (stderr, "Error in buffer allocation\n");
                        goto out;
                }
                memcpy (*chunk_buffer,
                        *buffer + start, *chunk_length);
        }
        ret = 0;
out:
        return ret;
}

/*Function to generate variable size chunk using rabin-karp.
Input:
        int fd          : File descriptor of file that to be chuncked
        int *ret        : Pointer to return 0 on success, -1 on failure
        int *size       : Poniter to return remaining size of the file
Output:
        char*           : Chunk to be returned
*/
char*
ydl_rabin_karp::get_variable_chunk (int fd, int *ret, int *size,
                                    int *chunk_flag, int *chunk_length)
{

        int     counter1                =       0;
        int     counter2                =       0;
        static int flag;
        int     remaining_flag          =       0;

        static char *buffer;
        static char *previous_remaining_block;

        char    *temp_buffer              =       NULL;
        char    *chunk_buffer             =       NULL;
        char    *remaining_buffer_content =       NULL;
        char    *remaining_window_content =       NULL;

        static y_uint32 hash;
        static y_uint32 power;

        static ssize_t start;
        static ssize_t end;
        static ssize_t remaining_length;
        static ssize_t buffer_length;
        static ssize_t previous_block_length;

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
                        if (remaining_length == 0 && buffer_length <= WINDOW_SIZE) {
                                *size = 0;
                                return buffer;
                        }

                        start                   =       0;
                        slide_incr              =       0;
                        end                     =       WINDOW_SIZE;

                        /*If there is remaining content in previous buffer,
                        set the end pointer of new buffer and
                        remaining_content_incr according to the window size*/
                        if (remaining_length > 0) {
                                if (remaining_length < WINDOW_SIZE) {
                                        end = WINDOW_SIZE - remaining_length;
                                        remaining_content_incr  = WINDOW_SIZE -
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
                                temp_buffer = (char *)calloc(1, WINDOW_SIZE + 1);
                                if (temp_buffer == NULL) {
                                        fprintf (stderr,
                                                "Error in buffer allocation\n");
                                        *ret     = -1;
                                        goto out;
                                }

                                if (remaining_length == WINDOW_SIZE &&
                                        remaining_content_incr != 0) {
                                        memcpy (temp_buffer,
                                                remaining_window_content,
                                                WINDOW_SIZE);
                                        hash = calc_hash (temp_buffer,
                                                &power, ret);
                                }

                                /*Creates the temp_buffer with previous buffers
                                remaining content and requried amount of data
                                from current buffer*/
                                if (remaining_length > 0
                                && remaining_content_incr != 0) {
                                        if (remaining_length < WINDOW_SIZE) {
                                                memcpy (temp_buffer,
                                                   remaining_buffer_content,
                                                   remaining_length);

                                                memcpy (temp_buffer + (WINDOW_SIZE-end),
                                                        buffer, end);
                                                remaining_flag = 1;
                                                hash = calc_hash (temp_buffer,
                                                        &power, ret);
                                        } else {
                                                remaining_flag = 0;
                                        }
                                }
                                /*Creates the temp_buffer of window size
                                from current buffer*/
                                else {
                                        memcpy (temp_buffer,
                                                buffer + start, WINDOW_SIZE);
                                        hash = calc_hash (temp_buffer,
                                                &power, ret);
                                }

                                if (*ret == -1) {
                                        fprintf (stderr,
                                                "Error calculating rolling hash\n");
                                        goto out;
                                }
                                clean_buff(&temp_buffer);
                        }

                        if (hash == FINGER_PRINT) {
                                *ret = get_chunk_buffer
                                (&remaining_content_incr,
                                 &remaining_length,
                                 &chunk_buffer, &buffer,
                                 &remaining_buffer_content,
                                 &remaining_window_content,
                                 start, end, slide_incr, chunk_length);

                                if (*ret == -1)
                                        goto out;
                                *chunk_flag      = 1;
                                slide_incr       = 0;
                                hash             = 0;
                                power            = 0;
                                flag             = 0;
                                start            = end;
                                remaining_length = buffer_length - end;
                                end += WINDOW_SIZE;
                                clean_buff(&previous_remaining_block);
                                previous_block_length = 0;
                                return chunk_buffer;
                        } else {

                                /*Sliding window when there is remaning content
                                  in previous buffer with remaining length
                                  less then window size*/
                                if (remaining_content_incr != 0 &&
                                    remaining_content_incr < WINDOW_SIZE &&
                                    remaining_flag == 1) {
                                        hash = (hash * PRIME - power *
                                        remaining_buffer_content[counter1] +
                                        buffer[end]) % M;
                                        remaining_content_incr++;
                                        counter1++;
                                } else {
                                        counter1 = 0;
                                }
                                /*Sliding window when there is remaning content
                                  in previous buffer with remaining length
                                  greater then window size*/
                                if (remaining_content_incr != 0 &&
                                    remaining_content_incr <= WINDOW_SIZE &&
                                    remaining_flag == 0) {
                                        hash = (hash * PRIME - power *
                                        remaining_window_content[counter2] +
                                        buffer[end]) % M;
                                        remaining_content_incr++;
                                        counter2++;
                                } else {
                                        counter2 = 0;
                                }
                                /*Sliding window with the current
                                          buffer*/
                                if ((remaining_content_incr == 0 ||
                                        remaining_content_incr >= WINDOW_SIZE) &&
                                        counter1 == 0 && counter2 == 0) {

                                        /*Returns the remaining content of
                                         previous buffer when there is no
                                        match with finger print even after
                                        sliding whole window to current buffer*/
                                        if (remaining_content_incr >= WINDOW_SIZE) {
                                                *chunk_length = remaining_length;
                                                remaining_length = buffer_length;
                                                clean_buff(&remaining_window_content);
                                                clean_buff(&previous_remaining_block);
                                                previous_block_length = 0;
                                                chunk_flag = 0;
                                                return remaining_buffer_content;
                                        }

                                        hash = (hash * PRIME - power *
                                        buffer[start+slide_incr] +
                                        buffer[end]) % M;
                                        slide_incr++;
                                }
                                end++;
                                flag = 1;
                        }
                }
                /*Keeps track of remaining buffers content which is not
                 matched with fingerprint*/

                if (remaining_length > 0) {
                        *ret = get_remaining_buffer_content
                                (&remaining_buffer_content,
                                &remaining_window_content, remaining_length,
                                &buffer, start);
                        if (*ret == -1)
                                goto out;
                }

                if (previous_block_length == 0 && remaining_length > 0) {
                        previous_remaining_block = (char *)calloc(1,
                        remaining_length + 1);
                        memcpy(previous_remaining_block,
                        remaining_buffer_content, remaining_length);
                        previous_block_length += remaining_length;
                }

                *size -= buffer_length;
                /*If buffer content is not matched with fingerprint and it has
                reached end of file, consider remaining buffer content as
                chunk*/
                if (remaining_length > 0 && *size == 0) {
                        *ret = get_chunk_buffer
                        (&remaining_content_incr,
                         &previous_block_length,
                         &chunk_buffer, &buffer,
                         &previous_remaining_block,
                         &remaining_window_content,
                         start, end, slide_incr, chunk_length);
                        if (remaining_length < WINDOW_SIZE)
                                *chunk_length = remaining_length;
                        clean_buff(&remaining_buffer_content);
                        *chunk_flag   = 1;
                        end             = 0;
                        start           = 0;
                        buffer_length   = 0;
                        hash            = 0;
                        power           = 0;
                        flag            = 0;
                        remaining_length = 0;
                        buffer_length   = 0;
                        previous_block_length = 0;
                        clean_buff(&buffer);
                        clean_buff(&remaining_window_content);
                        clean_buff(&remaining_buffer_content);
                        clean_buff(&previous_remaining_block);
                        return chunk_buffer;
                }
                end = 0;
                clean_buff(&buffer);
        }
        *ret = 0;
out:
        return NULL;
}
