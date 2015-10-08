#include <leveldb/c.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NAMESPACE_PATH "/var/lib/yadl/"
#define LENGTH 1024

struct namespace
{
        char *desc;
        char *hash_type;
        char *store_type;
        char *store_path;
        char *chunk_scheme;
        size_t chunk_size;
};
typedef struct namespace namespace;

/*@description: Function to create the namespace with given arguments.
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        creation of namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int
create_namespace(char *name, char *desc, char *hash_type, char *store_type,
                 char *store_path, char *chunk_scheme, int chunk_size);

int
create_namespace_file(char *filename, char *desc, char *hash_type,
                      char *store_type, char *store_path, char *chunk_scheme,
                      int chunk_size);