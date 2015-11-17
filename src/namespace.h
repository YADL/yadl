#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <string.h>
#include <getopt.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "min_hash.h"

enum OPTIONS {create, edit, delete_file, dedup, restore, info, list, reset, minhash, mrestore};

struct namespace_struct
{
        char *namespace_name;
        char *store_path;
        char *desc;
        char *hash_type;
        char *store_type;
        char *chunk_scheme;
        size_t  chunk_size;
};

typedef struct namespace_struct namespace_dtl;

#define LENGTH 1024
#define EX_LEN 5

/*@description:Function to to give correct instruction to use the various information.
Input:
        FILE *stream : Type of error.
        int exit_code : Exit code.
Output:
        void.
*/
void print_usage (FILE *stream, int exit_code);

/*@description: Function to take the command line argument and assign the values
  for perticular namespace and assigns perticular operations.
Input:
        int argc : Number of argument.
        char **argv : Command line arguments.
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int start_program(int argc, char **argv, char *namespace_path);

/*@description: Function to dedup the file .
Input:
        namespace_dtl namespace_input : Takes of the information of namespace
        char *file_path : Path of the file to be deduped
Output:
        int : Return 0 on success -1 on failure.
*/
int dedup_file (namespace_dtl namespace_input,char *file_path);

/*@description: Function to call different file operation functions with vaid inputs.
 initiate various stores.
Input:
        enum OPTIONS flag : Notifies which file operation to be performed.
        char *filename : File that to be operated.
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        file operations.
Output:
        int : Return 0 on success -1 on failure.
*/
int file_operation(enum OPTIONS flag, char *filename, char *namespace_path,
namespace_dtl set_namespace, minhash_config minhash_config_dtl);

/*@description: Function to create the namespace with given arguments.
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        creation of namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int create_namespace(char *namespace_path, namespace_dtl set_namespace);

/*@description: Function to display the info of perticular namespace and info of
 all namespace.
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information.
Output:
        int : Return 0 on success -1 on failure.
*/
int namespace_info(char *namespace_path, namespace_dtl set_namespace);

/*@description: Function to list the namespace that are created.
Input:
        int argc : Number of arguments.
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int list_namespace(int argc, char *namespace_path);

/*@description: Function to delete the namespace.
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        delete operations.
Output:
        int : Return 0 on success -1 on failure.
*/
int delete_namespace(char *namespace_path, namespace_dtl set_namespace);

/*@description: Function to reset the namespace by deleting all stores of
 perticular namespace.
Input:
        char *store_path : Path of the store.
Output:
        int : Return 0 on success -1 on failure.
*/
int clear_store(char *store_path);

/*@description: Function to edit the namespace
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        edit operations.
Output:
        int : Return 0 on success -1 on failure.
*/
int edit_namespace(char *namespace_path, namespace_dtl set_namespace);

/*@description: Function to get the namespace configuration
Input:
        char *buffer : Path of the namespace.
        int *ret : Return value 0 on success -1 on failure.
Output:
        namespace_dtl set_namespace : Contains namespace information to perform
                                        edit operations.
*/
namespace_dtl get_namespace_method(char *buffer, int *ret);

/*@description: Function to create the default namespace
Input:
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int create_default_namespace(char *namespace_path);