#include "md5.h"
#include "vector.h"

/*Function to implement md5.
Input:char *buffer
Output:DIGEST
*/
DIGEST *str2md5(vector_ptr list)
{

        MD5_CTX c;
        DIGEST *digest = (DIGEST *)calloc(1, 16);
        vector_ptr temp_node;

        if (list != NULL) {
                MD5_Init(&c);
                temp_node = list;
                do {
                        MD5_Update(&c, temp_node->vector_element,
                                temp_node->length);
                        temp_node = temp_node->next;
                } while (temp_node != NULL);

                MD5_Final(digest, &c);
        }
        return digest;

}