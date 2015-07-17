#include "md5.h"

/*Function to implement md5.
Input:vector_ptr
Output:char*
*/
char *str2md5(vector_ptr list)
{

        int n;
        MD5_CTX c;
        unsigned char digest[16];
        char *out = (char *)calloc(1, 33);
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

                for (n = 0; n < 16; ++n) {
                        snprintf(&(out[n*2]), 16*2, "%02x",
                                (unsigned int)digest[n]);
                }
        }
        return out;

}
