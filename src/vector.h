#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

/*Structure to store buffer contents and the length until the chunk is found*/
struct vector
{
        void *vector_element;
        ssize_t length;
        struct vector *next;
};
typedef struct vector *vector_ptr;

/*@description:Function to create a new node of type vector_ptr.
Input: 
        void
Output:
        vector_ptr     : New node of type vector_ptr
*/
vector_ptr getnode();

/*description:Function to insert buffer content to vector.
Input:
        void *data      : Holds the buffer content
        vector_ptr list : Vector to store the buffer content
        int *ret        : Returns the exit status
Output:
        vector_ptr      : Vector that contains the buffer content
*/
vector_ptr insert_vector_element(void *data, vector_ptr list, int *ret, int length);
