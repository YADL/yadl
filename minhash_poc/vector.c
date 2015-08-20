#include "vector.h"

/*description:Function to create a new node of type vector_ptr.
Input:
        void
Output:
        vector_ptr     : New node of type vector_ptr
*/
vector_ptr
getnode()
{
        vector_ptr new_node = NULL;

        new_node = (vector_ptr)malloc(sizeof(vector_ptr));
        return new_node;
}

/*description:Function to insert buffer content to vector.
Input:
        void *data      : Holds the buffer content
        vector_ptr list : Vector to store the buffer content
        int *ret        : Returns the exit status
Output:
        vector_ptr      : Vector that contains the buffer content
*/
vector_ptr
insert_vector_element(void *data, vector_ptr list, int *ret, int length)
{

        vector_ptr      new_node        =       NULL;
        vector_ptr      temp_node       =       NULL;

        new_node = getnode();
        if (new_node == NULL) {
                *ret = -1;
                goto out;
        }
        new_node->length = length;
        new_node->vector_element = (void *)calloc(1, new_node->length + 1);
        if (new_node->vector_element == NULL) {
                *ret = -1;
                goto out;
        }
        memcpy(new_node->vector_element, data, new_node->length);
        new_node->next = NULL;
        if (list != NULL) {
                for (temp_node = list; temp_node->next != NULL;
                temp_node = temp_node->next)
                        ;
                temp_node->next = new_node;
        } else
                list = new_node;
out:
        return list;
}

