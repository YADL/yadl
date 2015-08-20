#include "sha1.h"

/*Function to create hash using sha1
Input:char *buffer
Output:DIGEST
*/
DIGEST *sha1(char *buffer)
{

        DIGEST *digest = (DIGEST *)calloc(1, 20);
        SHA_CTX c;

        SHA1_Init(&c);
        SHA1_Update(&c, buffer, strlen(buffer));
        SHA1_Final(digest, &c);

        return digest;

}