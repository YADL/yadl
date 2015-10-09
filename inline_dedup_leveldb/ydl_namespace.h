#include <leveldb/c.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NAMESPACE_PATH "/var/lib/yadl/namespaces/"

struct namespace
{
        char *desc;
        char *hash_type;
        char *store_type;
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
int create_namespace(char *ns_name, char *ns_desc, char *ns_hash_type,
                                char *ns_store_type, char *ns_chunk_scheme,
                                char *ns_chunk_size);

/*@description: Function to edit the namespace
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        edit operations.
Output:
        int : Return 0 on success -1 on failure.
*/
int edit_namespace(char *ns_name);

/*@description: Function to delete the namespace.
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        delete operations.
Output:
        int : Return 0 on success -1 on failure.
*/
int delete_namespace(char *ns_name);

/*@description: Function to reset the namespace by deleting all stores of
 perticular namespace.
Input:
        char *store_path : Path of the store.
Output:
        int : Return 0 on success -1 on failure.
*/
int clear_store(char *store_path);

/*@description: Function to list the namespace that are created.
Input:
        int argc : Number of arguments.
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int list_namespace();

/*@description: Function to get the namespace configuration
Input:
        char *buffer : Path of the namespace.
        int *ret : Return value 0 on success -1 on failure.
Output:
        namespace_dtl set_namespace : Contains namespace information to perform
                                        edit operations.
*/
namespace* open_namespace(char *ns_name);

int write_to_namespace(leveldb_t *db, leveldb_writeoptions_t *write_opt,
                        char *key, char *value);

char *read_from_namespace(leveldb_t *db, leveldb_readoptions_t *read_opt, char *key);