#include "sha1.h"

/*Function to create hash using sha1
Input:char str[],int length
Output:char*
*/
char* sha1(char str[],int length)
{
        
        char *out       =       (char*)calloc(1,41);
        unsigned char hash[SHA_DIGEST_LENGTH]; 
        
        SHA1((unsigned char *)str, length, hash);
        for (length = 0; length < SHA_DIGEST_LENGTH; ++length)
                snprintf(&(out[length*2]), SHA_DIGEST_LENGTH*2, "%02x", (unsigned int)hash[length]);
        return out;
        
}
