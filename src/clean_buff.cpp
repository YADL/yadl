#include "clean_buff.h"

/*Function to clean buffer contents.
Input:char* path
Output:int
*/
void clean_buff(char **buffer)
{

        if (*buffer != NULL) {
                free(*buffer);
                *buffer = NULL;
        }

}
