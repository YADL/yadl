#include "sha1.h"

/*Function to create hash using sha1
Input:char str[],int length
Output:char*
*/
char* sha1(vector_ptr list)
{
        int n = 0;
        char *out       =       (char*)calloc(1,41);
        SHA_CTX c;
        unsigned char hash[SHA_DIGEST_LENGTH]; 
        vector_ptr temp_node;

        if(list != NULL){
                SHA1_Init(&c);
                temp_node = list;
                do {
                        SHA1_Update(&c, temp_node->vector_element, temp_node->length);
                        temp_node=temp_node->next;
                }while(temp_node!=NULL);

                SHA1_Final(hash, &c);

                for (n = 0; n < SHA_DIGEST_LENGTH; ++n)
                {
                        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)hash[n]);
                }
        }
        return out;

}
