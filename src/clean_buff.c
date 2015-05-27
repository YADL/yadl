#include "clean_buff.h"
#include<stdio.h>
#include<string.h>

/*Function to clean buffer contents.
Input:char* path
Output:int
*/
inline void clean_buff(char** buffer)
{
        
        if(*buffer!=NULL)
        {
                free(*buffer);
                *buffer=NULL;
        }

}

