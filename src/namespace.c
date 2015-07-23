#include "namespace.h"
#include "block.h"
#include "catalog.h"
#include "hash.h"
#include "restore.h"
#include "stub.h"

/*Function to to give correct instruction to use the various information.
Input:
        FILE *stream : Type of error.
        int exit_code : Exit code.
Output:
        void.
*/
void
print_usage (FILE *stream, int exit_code)
{
        fprintf(stream,
                "\n -c --create      Create new namespace\n"
                " -n --namespace   Create namespace file\n"
                " -p --store_path  Path of the store\n"
                " -h --hash_type   Type of hash\n"
                " -s --store_type  Type of store\n"
                " --chunk_scheme   Type of chunk\n"
                " --chunk_size     Size of chunk is it is fixed chunk_scheme\n"
                " --desc           Description of namespace\n"
                " -i --info        Display all the information of namespace\n"
                "                  To Display information of all namespace use 'all'\n"
                "                  as argument for namespace\n"
                " -l --list        List all namespace or catalog of perticular namespace\n"
                " -R --reset       Clear all the stores of perticular namespace\n"
                " -d --delete      Delete perticular namespace or perticular file\n"
                " --dedup          Dedup file\n"
                " -r --restore     Restore file\n"
                " -f --file        File path to perform various file operation\n"
                " --help           Prints usage\n"
                "\nNamespace operation:\n====================\n"
                "Creat namespace:\n"
                "$> yadl --create/-c  -n <namespace_name> --store_path <store_path>\n"
                "[--hash_type {md5/sha1}]\n"
                "[--chuck_scheme {variable/fixed} [--chuck_size <chuck_size>]  ]\n"
                "[--store_type {default/object}] [--desc <namespace_description>]\n"
                "\nInfo of namespace:\n"
                "$> yadl --info/-i -n <namespace_name>\n"
                "$> yadl --info/-i -n all\n"
                "\nList all namespace:\n"
                "$> yadl --list/-l\n"
                "\nEdit namespace:\n"
                "$> yadl --edit/-e -n <namespace_name>"
                        " [--desc <namespace_description>]\n"
                "\nReset namespace:\n"
                "$> yadl --reset/-R -n <namespace_name>\n"
                "\nDelete namespace:\n"
                "$> yadl --delete/-d -n <namespace_name>\n"
                "\nFile operations:\n================\n"
                "Dedup file:\n"
                "$> yadl --dedup -n <namespace_name> --file/-f <file_path>\n"
                "\nRestore file:\n"
                "$> yadl --restore/-r -n <namespace_name> --file/-f <file_path>\n"
                "\nDelete file:\n"
                "$> yadl --delete/-d -n <namespace_name> --file/-f <file_path>\n"
                "\nList files in namespace:\n"
                "$> yadl --list/-l -n <namespace_name>\n\n"
                );
}

/*Function to create the default namespace
Input:
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int
create_default_namespace(char *namespace_path)
{
        int     ret                     =       -1;
        int     fd                      =       -1;
        char    file_path[LENGTH]       =       "";
        char    content[LENGTH]         =       "";
        DIR     *store_dp               =       NULL;

        if (namespace_path == NULL) {
                printf("Invalid namespace path\n");
                goto out;
        }
        sprintf(file_path, "%s/default.yadl", namespace_path);
        fd = open(file_path, O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        sprintf(content, "store_path:/var/lib\n");
        sprintf(content, "%sstore_type:default\n", content);
        sprintf(content, "%shash_type:md5\n", content);
        sprintf(content, "%schunk_scheme:variable\n", content);
        sprintf(content, "%schunk_size:0\n", content);
        sprintf(content, "%sdesc:Default namespace\n", content);
        ret = write (fd, content, strlen(content));
        if (ret < 0)
                goto out;

        store_dp = opendir("/var/lib");
        if (NULL == store_dp) {
                ret = mkdir("/var/lib", 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        ret = 0;
out:
        if (ret < 0)
                print_usage(stderr, 1);
        if (fd != -1)
                close(fd);
        return ret;
}


/*Function to create the namespace with given arguments.
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        creation of namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int
create_namespace(char *namespace_path, namespace_dtl set_namespace)
{
        int     ret                     =       -1;
        int     fd                      =       -1;
        char    file_path[LENGTH]       =       "";
        char    content[LENGTH]         =       "";
        char    store_path[LENGTH]      =       "";
        char    namespace_filename[LENGTH] =    "";
        char    *buffer                 =       NULL;
        DIR     *dp                     =       NULL;
        DIR     *store_dp               =       NULL;
        namespace_dtl get_namespace;
        struct stat st;

        if (namespace_path == NULL || set_namespace.store_path == NULL) {
                printf("Invalid argument list\n");
                goto out;
        }

        if (namespace_path != NULL && set_namespace.store_path != NULL &&
                set_namespace.store_type == NULL &&
                set_namespace.hash_type == NULL &&
                set_namespace.chunk_scheme == NULL &&
                set_namespace.chunk_size == 0) {

                sprintf(namespace_filename, "%s/default.yadl", namespace_path);
                fd = open(namespace_filename, O_RDONLY, S_IWUSR|S_IRUSR);
                if (fd < 1) {
                        fprintf(stderr, "%s\nNamespace does not exists\n",
                                strerror(errno));
                        goto out;
                }
                buffer = (char *)calloc(1, LENGTH);
                ret = read(fd, buffer, LENGTH);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                get_namespace = get_namespace_method(buffer, &ret);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                set_namespace.store_type = get_namespace.store_type;
                set_namespace.hash_type = get_namespace.hash_type;
                set_namespace.chunk_scheme = get_namespace.chunk_scheme;
                set_namespace.chunk_size = set_namespace.chunk_size;
                printf("Default namespace configure is assigning...\n");
        } else if (set_namespace.store_type == NULL ||
                set_namespace.hash_type == NULL ||
                set_namespace.chunk_scheme == NULL) {
                printf("Invalid argument list\n");
                goto out;
        }

        if (set_namespace.namespace_name == NULL) {
                printf("Invalid Namespace name\n");
                goto out;
        }
        if (strcmp(set_namespace.namespace_name, "all") == 0) {
                printf("all is command so it cannot be used as namespace\n");
                goto out;
        }
        sprintf(file_path, "%s/%s.yadl", namespace_path,
                set_namespace.namespace_name);
        if (stat (file_path, &st) != 0) {

                dp = opendir(set_namespace.store_path);
                if (dp == NULL) {
                        printf("Invalid store path\n");
                        goto out;
                } else {
                        closedir(dp);
                        sprintf(store_path, "%s/store_block",
                                set_namespace.store_path);
                        dp = opendir(store_path);
                        if (dp != NULL) {
                                fprintf(stderr,
                                "\nGiven store path is already occupied by "
                                "other namespace\n\n");
                                goto out;
                        }
                }

                if (strcmp(set_namespace.store_type, "default") != 0 &&
                strcmp(set_namespace.store_type, "object") != 0) {
                        printf("Invalid store type\n");
                        goto out;
                }

                if (strcmp(set_namespace.hash_type, "md5") != 0 &&
                strcmp(set_namespace.hash_type, "sha1") != 0) {
                        printf("Invalid hash type\n");
                        goto out;
                }

                if (strcmp(set_namespace.chunk_scheme, "fixed") != 0 &&
                strcmp(set_namespace.chunk_scheme, "variable") != 0) {
                        printf("Invalid chunk_scheme\n");
                        goto out;
                }

                if (strcmp(set_namespace.chunk_scheme, "fixed") == 0 &&
                set_namespace.chunk_size < 1) {
                        printf("Invalid chunk_size\n");
                        goto out;
                }

                if (strcmp(set_namespace.chunk_scheme, "variable") == 0 &&
                set_namespace.chunk_size > 0) {
                        printf("Variable chunk_scheme does not accept "
                        "chunk_size\n");
                        goto out;
                }
                fd = open(file_path, O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
                if (fd < 1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                sprintf(content, "store_path:%s\n", set_namespace.store_path);
                sprintf(content, "%sstore_type:%s\n", content,
                        set_namespace.store_type);
                sprintf(content, "%shash_type:%s\n", content,
                        set_namespace.hash_type);
                sprintf(content, "%schunk_scheme:%s\n", content,
                        set_namespace.chunk_scheme);
                sprintf(content, "%schunk_size:%lu\n", content,
                        set_namespace.chunk_size);
                sprintf(content, "%sdesc:%s\n", content, set_namespace.desc);
                ret = write (fd, content, strlen(content));
                if (ret < 0)
                        goto out;
                printf("namespace created %s\n", file_path);
                memset(store_path, 0, LENGTH);
                sprintf(store_path, "%s/store_block", set_namespace.store_path);
                store_dp = opendir(store_path);
                if (NULL == store_dp) {
                        ret = mkdir(store_path, 0777);
                        if (ret < 0) {
                                fprintf(stderr, "%s\n", strerror(errno));
                                goto out;
                        }
                }
                printf("store created %s\n", store_path);
        } else {
                printf("namespace already present\n");
        }
        ret = 0;
out:
        if (ret < 0)
                print_usage(stderr, 1);
        if (fd != -1)
                close(fd);
        if (dp != NULL)
                closedir(dp);
        return ret;
}

/*Function to display the info of perticular namespace and info of
 all namespace.
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information
Output:
        int : Return 0 on success -1 on failure.
*/
int
namespace_info(char *namespace_path, namespace_dtl set_namespace)
{
        int     ret             =       -1;
        int     fd              =       -1;
        int     count           =        0;
        char    filename[LENGTH]  =       "";
        char    content[LENGTH]   =       "";
        DIR           *dp       =       NULL;
        struct dirent *dir;

        if (set_namespace.namespace_name == NULL) {
                goto out;
        }
        if (namespace_path == NULL) {
                goto out;
        }

        if (strcmp(set_namespace.namespace_name, "all") != 0) {
                sprintf(filename, "%s/%s.yadl", namespace_path,
                        set_namespace.namespace_name);
                printf("%s :\n", filename);
                fd = open(filename, O_RDONLY, S_IRUSR|S_IWUSR);
                if (fd < 1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                ret = read(fd, content, LENGTH);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                printf("\n%s\n", content);
        } else {
                dp = opendir(namespace_path);
                if (dp) {
                        while ((dir = readdir(dp)) != NULL) {
                                if (strcmp(dir->d_name, ".") != 0 &&
                                strcmp(dir->d_name, "..") != 0) {
                                        sprintf(filename, "%s/%s",
                                                namespace_path, dir->d_name);
                                        printf("\n%s:", filename);
                                        fd = open(filename, O_RDONLY,
                                                S_IRUSR|S_IWUSR);
                                        if (fd < 1) {
                                                fprintf(stderr, "%s\n",
                                                        strerror(errno));
                                                goto out;
                                        }
                                        ret = read(fd, content, LENGTH);
                                        if (ret < 0) {
                                                fprintf(stderr, "%s\n",
                                                        strerror(errno));
                                                goto out;
                                        }
                                        printf("\n%s\n", content);
                                        memset(content, 0, LENGTH);
                                        count++;
                                }
                        }
                        if (count == 0) {
                                printf("No namespace found\n");
                        }
                }
        }
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        if (fd != -1)
                close(fd);
        if (ret < 0)
                print_usage(stderr, 1);
        return ret;
}

/*Function to list the namespace that are created.
Input:
        int argc : Number of arguments.
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int
list_namespace(int argc, char *namespace_path)
{

        int     ret               =       -1;
        int     count             =        0;
        char    filename[LENGTH]  =       "";
        DIR           *dp         =       NULL;
        struct dirent *dir;

        if (namespace_path == NULL) {
                goto out;
        }
        if (argc != 2) {
                goto out;
        }
        dp = opendir(namespace_path);
        if (dp) {
                while ((dir = readdir(dp)) != NULL) {
                        if (strcmp(dir->d_name, ".") != 0 &&
                                strcmp(dir->d_name, "..") != 0) {
                                sprintf(filename, "%s/%s",
                                        namespace_path, dir->d_name);
                                printf("\n%s\n", filename);
                                count++;
                        }
                }
                if (count == 0) {
                        printf("No namespace found\n");
                }
        } else {
                goto out;
        }
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        if (ret < 0)
                print_usage(stderr, 1);
        return ret;
}

/*Function to edit the namespace
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        edit operations.
Output:
        int : Return 0 on success -1 on failure.
*/
int
edit_namespace(char *namespace_path, namespace_dtl set_namespace)
{
        int     ret               =       -1;
        int     fd                =       -1;
        char    filename[LENGTH]  =       "";
        char    content[LENGTH]   =       "";
        char    replace[LENGTH]   =       "";
        char    buffer[LENGTH]    =       "";
        char    *ptr              =       NULL;

        if (set_namespace.store_path != NULL ||
                set_namespace.store_type != NULL ||
                set_namespace.hash_type != NULL ||
                set_namespace.chunk_scheme != NULL) {
                printf("Only mutable parameters can be edited\n");
                goto out;
        }

        if (set_namespace.namespace_name == NULL || set_namespace.desc == NULL ||
                namespace_path == NULL) {
                goto out;
        }
        sprintf(replace, "desc:%s", set_namespace.desc);
        sprintf(filename, "%s/%s.yadl", namespace_path,
                set_namespace.namespace_name);
        printf("%s :\n", filename);
        fd = open(filename, O_RDONLY, S_IRUSR|S_IWUSR);
        if (fd < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ret = read(fd, content, LENGTH);
        if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ptr = strstr(content, "desc:");
        if (!ptr)
                goto out;

        strncpy(buffer, content, ptr-content);
        buffer[ptr-content] = '\0';
        sprintf(buffer+(ptr-content), "%s", replace);
        printf("%s\n", buffer);
        if (fd != -1)
                close(fd);
        fd = open(filename, O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
        if (fd < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        if (-1 == write(fd, buffer, strlen(buffer))) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        ret = 0;
out:
        if (fd != -1)
                close(fd);
        if (ret < 0)
                print_usage(stderr, 1);
        return ret;
}

/*Function to delete the namespace.
Input:
        char *namespace_path : Path of the namespace.
        namespace_dtl set_namespace : Contains namespace information to perform
                                        delete operations.
Output:
        int : Return 0 on success -1 on failure.
*/
int
delete_namespace(char *namespace_path, namespace_dtl set_namespace)
{

        int     ret                     =       -1;
        int     count                   =        0;
        char    namespace_file[LENGTH]  =       "";
        char    filename[LENGTH]        =       "";
        DIR           *dp               =       NULL;
        struct dirent *dir;

        if (namespace_path == NULL) {
                goto out;
        }
        if (set_namespace.namespace_name == NULL) {
                goto out;
        }

        if (strcmp(set_namespace.namespace_name, "default") == 0) {
                printf("Sorry!! Default settings cannot be deleted\n");
                ret = 0;
                goto out;
        }

        sprintf(namespace_file, "%s.yadl", set_namespace.namespace_name);
        dp = opendir(namespace_path);
        if (dp) {
                while ((dir = readdir(dp)) != NULL) {
                        if (strcmp(dir->d_name, ".") != 0 &&
                                strcmp(dir->d_name, "..") != 0) {
                                if (strcmp(dir->d_name, namespace_file) == 0) {
                                        sprintf(filename, "%s/%s",
                                                namespace_path, namespace_file);
                                        ret = file_operation(reset, "delete",
                                                namespace_path, set_namespace);
                                        if (ret == -1 || ret == 1) {
                                                goto out;
                                        }
                                        ret = remove(filename);
                                        if (ret == -1) {
                                                fprintf(stderr, "%s\n",
                                                        strerror(errno));
                                                goto out;
                                        }
                                        count++;
                                }
                        }
                }
                if (count == 0) {
                        printf("Namespace does not exists\n");
                }
        } else {
                goto out;
        }
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        if (ret < 0)
                print_usage(stderr, 1);
        return ret;
}

/*@description: Function to reset the namespace by deleting all stores of
 perticular namespace.
Input:
        char *store_path : Path of the store.
Output:
        int : Return 0 on success -1 on failure.
*/
int
clear_store(char *store_path)
{
        int ret           =         -1;
        char path[LENGTH] =         "";
        char confirm      =        'n';
        DIR     *dp       =       NULL;

        if (store_path == NULL) {
                goto out;
        }

        sprintf(path, "%s/store_block", store_path);
        dp = opendir(path);
        if (NULL == dp) {
                fprintf(stderr, "\nNamespace does not contain any stores\n\n");
                goto out;
        }

        printf("Do you want to continue with deletion?[y/n]");
        if (scanf("%c", &confirm) <= 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
        }
        if (confirm == 'y' || confirm == 'Y') {
                memset(path, 0, LENGTH);
                sprintf(path, "rm -rf %s/store_block", store_path);
                ret = system(path);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        return ret;
}

/*Function to get the namespace configuration
Input:
        char *buffer : Path of the namespace.
        int *ret : Return value 0 on success -1 on failure.
Output:
        namespace_dtl set_namespace : Contains namespace information to perform
                                        edit operations.
*/
namespace_dtl
get_namespace_method(char *buffer, int *ret)
{
        char    *token1         =       NULL;
        char    *token2         =       NULL;
        char    *str            =       NULL;
        char    *sub_str        =       NULL;
        char    *key_value[3];
        const char dlmtr1[2]    =       ":";
        const char dlmtr2[2]    =       "\n";
        char    *saveptr1       =       NULL;
        char    *saveptr2       =       NULL;
        int     index           =       0;
        namespace_dtl get_namespace;

        *ret    =       -1;
        for (str = buffer ; ; str = NULL) {
                token1 = strtok_r(str, dlmtr2, &saveptr1);
                if (token1 == NULL)
                        break;

                for (sub_str = token1 ; ; sub_str = NULL) {
                        token2 = strtok_r(sub_str, dlmtr1, &saveptr2);
                        if (token2 == NULL)
                                break;
                        key_value[index] = token2;
                        index++;
                }
                if (strcmp(key_value[0], "store_path") == 0) {
                        get_namespace.store_path = key_value[1];
                        if (get_namespace.store_path == NULL) {
                                goto out;
                        }
                }
                if (strcmp(key_value[0], "store_type") == 0) {
                        get_namespace.store_type = key_value[1];
                        if (get_namespace.store_type == NULL) {
                                goto out;
                        }
                }
                if (strcmp(key_value[0], "hash_type") == 0) {
                        get_namespace.hash_type = key_value[1];
                        if (get_namespace.hash_type == NULL) {
                                goto out;
                        }
                }
                if (strcmp(key_value[0], "chunk_scheme") == 0) {
                        get_namespace.chunk_scheme = key_value[1];
                        if (get_namespace.chunk_scheme == NULL) {
                                goto out;
                        }
                }
                if (strcmp(key_value[0], "chunk_size") == 0) {
                        get_namespace.chunk_size = atoi(key_value[1]);
                }
                index = 0;
        }
        *ret = 0;
out:
        return get_namespace;
}

/*Function to call different file operation functions with vaid inputs.
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
int
file_operation(enum OPTIONS flag, char *filename, char *namespace_path,
namespace_dtl set_namespace)
{
        char    *buffer         =       NULL;
        DIR     *dp             =       NULL;
        int     ret             =       -1;
        int     fd              =       -1;
        char    namespace_filename[LENGTH];
        char    path[LENGTH]    =       "";
        char    confirm         =       -1;
        static namespace_dtl get_namespace;

        if (namespace_path == NULL) {
                goto out;
        }

        if (set_namespace.namespace_name == NULL) {
                printf("Namespace not specified");
                goto out;
        }

        sprintf(namespace_filename, "%s/%s.yadl", namespace_path,
                set_namespace.namespace_name);
        fd = open(namespace_filename, O_RDONLY, S_IWUSR|S_IRUSR);
        if (fd < 1) {
                fprintf(stderr, "%s\nNamespace does not exists\n",
                        strerror(errno));
                goto out;
        }
        buffer = (char *)calloc(1, LENGTH);
        ret = read(fd, buffer, LENGTH);
        if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        get_namespace = get_namespace_method(buffer, &ret);
        if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        sprintf(path, "%s/store_block", get_namespace.store_path);
        dp = opendir(path);
        if (NULL == dp) {
                ret = mkdir(path, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        ret = init_block_store(path);
        if (ret == -1)
                goto out;
        ret = init_hash_store(path);
        if (ret == -1)
                goto out;
        ret = init_catalog_store(path);
        if (ret == -1)
                goto out;
        switch (flag) {
        case dedup:
                if (filename == NULL) {
                        printf("File name requried :"
                        "Try $>yadl --help for more information\n");
                        goto out;
                }
                ret = dedup_file(get_namespace, filename);
                if (ret < 0)
                        goto out;
                break;
        case restore:
                if (filename == NULL) {
                        printf("File name requried :"
                        "Try $>yadl --help for more information\n");
                        goto out;
                }
                ret = restore_file(filename, get_namespace.store_path);
                if (ret < 0)
                        goto out;
                break;
        case delete_file:
                printf("Do you want to continue with deletion?[y/n]");
                if (scanf("%c", &confirm) <= 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                if (confirm == 'y' || confirm == 'Y') {
                        if (filename == NULL) {
                                printf("File name requried :"
                                "Try $>yadl --help for more information\n");
                                goto out;
                        }
                        ret = delete_stub_store(get_namespace.store_path,
                                filename);
                        if (ret < 0)
                                goto out;
                        printf("\nStub deleted\n");
                }
                break;
        case list:
                ret = readfilecatalog();
                if (ret < 0)
                        goto out;
                break;
        case reset:
                ret = clear_store(get_namespace.store_path);
                if (ret < 0)
                        goto out;
                dp = opendir(path);
                if (NULL != dp) {
                        ret = 1;
                        goto out;
                }
                if (filename != NULL) {
                        if (strcmp(filename, "delete") == 0)
                                goto out;
                }
                ret = mkdir(path, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                break;
        default:
                break;
        }
        ret = fini_block_store();
        if (ret == -1)
                goto out;
        ret = fini_hash_store();
        if (ret == -1)
                goto out;
        ret = fini_catalog_store();
        if (ret == -1)
                goto out;
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        if (fd != -1)
                close(fd);
        return ret;
}

/*Function to take the command line argument and assign the values
  for perticular namespace and assigns perticular operations.
Input:
        int argc : Number of argument.
        char **argv : Command line arguments.
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int
start_program(int argc, char **argv, char *namespace_path)
{

        int     choice                  =       -1;
        int     ret                     =       -1;
        char    *file_path              =     NULL;
        enum    OPTIONS flag            =       -1;
        int     option_index            =        0;
        int     i                       =        0;
        static namespace_dtl set_namespace;

        const char *short_options = "cn:p:h:s:df:ilRre";

        const struct option long_options[] = {
                {"create",          no_argument,            0,   'c'},
                {"namespace",       required_argument,      0,   'n'},
                {"store_path",      required_argument,      0,   'p'},
                {"hash_type",       required_argument,      0,   'h'},
                {"store_type",      required_argument,      0,   's'},
                {"chunk_scheme",    required_argument,      0,   0 },
                {"chunk_size",      required_argument,      0,   0 },
                {"desc",            required_argument,      0,   0 },
                {"dedup",           no_argument,            0,   0 },
                {"file",            required_argument,      0,   'f'},
                {"restore",         no_argument,            0,   'r'},
                {"delete",          no_argument,            0,   'd'},
                {"info",            no_argument,            0,   'i'},
                {"list",            no_argument,            0,   'l'},
                {"reset",           no_argument,            0,   'R'},
                {"edit",            no_argument,            0,   'e'},
                {"help",            no_argument,            0,   0},
                {0,                 0,                      0,   0 }
        };

        if (argc == 1) {
                print_usage (stderr, 1);
                goto out;
        }
        while (1) {
                option_index = 0;

                choice = getopt_long(argc, argv, short_options,
                         long_options, &option_index);
                if (choice == -1) {
                        break;
                }
                switch (choice) {
                case 0:
                        if (strcmp(long_options[option_index].name,
                        "chunk_scheme") == 0) {
                                set_namespace.chunk_scheme = optarg;
                        }
                        if (strcmp(long_options[option_index].name,
                        "chunk_size") == 0) {
                                for (i = 0; optarg[i]; i++) {
                                        if (!isdigit(optarg[i])) {
                                                printf("Invalid chunk size\n");
                                                ret = -1;
                                                goto out;
                                        }
                                }
                                set_namespace.chunk_size = atoi(optarg);
                        }
                        if (strcmp(long_options[option_index].name,
                        "desc") == 0) {
                                set_namespace.desc = optarg;
                        }
                        if (strcmp(long_options[option_index].name,
                        "dedup") == 0) {
                                if (flag != -1) {
                                        printf("Trying to use more than one operation:"
                                        "Try $>yadl --help for more information\n");
                                        goto out;
                                }
                                flag = dedup;
                        }
                        if (strcmp(long_options[option_index].name,
                        "help") == 0) {
                                print_usage(stderr, 1);
                                goto out;
                        }
                        break;

                case 'c':
                        if (flag != -1) {
                                printf("Trying to use more than one operation :"
                                "Try $>yadl --help for more information\n");
                                goto out;
                        }
                        flag = create;
                        break;

                case 'n':
                        set_namespace.namespace_name = optarg;
                        break;

                case 'p':
                        set_namespace.store_path = optarg;
                        break;

                case 's':
                        set_namespace.store_type = optarg;
                        break;

                case 'h':
                        set_namespace.hash_type = optarg;
                        break;

                case 'd':
                        if (flag != -1) {
                                printf("Trying to use more than one operation :"
                                "Try $>yadl --help for more information\n");
                                goto out;
                        }
                        flag = delete_file;
                        break;

                case 'r':
                        if (flag != -1) {
                                printf("Trying to use more than one operation :"
                                "Try $>yadl --help for more information\n");
                                goto out;
                        }
                        flag = restore;
                        break;

                case 'f':
                        file_path = optarg;
                        break;

                case 'i':
                        if (flag != -1) {
                                printf("Trying to use more than one operation :"
                                "Try $>yadl --help for more information\n");
                                goto out;
                        }
                        flag = info;
                        break;

                case 'l':
                        if (flag != -1) {
                                printf("Trying to use more than one operation :"
                                "Try $>yadl --help for more information\n");
                                goto out;
                        }
                        flag = list;
                        break;

                case 'R':
                        if (flag != -1) {
                                printf("Trying to use more than one operation :"
                                "Try $>yadl --help for more information\n");
                                goto out;
                        }
                        flag = reset;
                        break;

                case 'e':
                        flag = edit;
                        break;

                case '?':
                        print_usage (stderr, 1);
                        goto out;

                default:
                        printf("?? getopt returned character code 0%o ??\n",
                                choice);
                }
        }

        if (optind < argc) {
                printf("non-option ARGV-elements:");
                while (optind < argc)
                        printf("%s\n", argv[optind++]);
                print_usage(stderr, 1);
                goto out;
        }
        switch (flag) {
        case create:
                ret = create_namespace(namespace_path, set_namespace);
                if (ret == -1)
                        goto out;
                break;
        case edit:
                ret = edit_namespace(namespace_path, set_namespace);
                if (ret == -1)
                        goto out;
                break;
        case delete_file:
                if (file_path == NULL) {
                        ret = delete_namespace(namespace_path, set_namespace);
                        if (ret < 0) {
                                goto out;
                        }
                        break;
                }
        case dedup:
        case restore:
        case list:
                if (set_namespace.namespace_name == NULL) {
                        ret = list_namespace(argc, namespace_path);
                        if (ret == -1)
                                goto out;
                        break;
                }
        case reset:
                ret = file_operation(flag, file_path, namespace_path,
                        set_namespace);
                if (ret == -1)
                        goto out;
                break;
        case info:
                ret = namespace_info(namespace_path, set_namespace);
                if (ret == -1)
                        goto out;
                break;
        default:
                print_usage (stderr, 1);
                goto out;
        }
        ret = 0;
out:
        return ret;
}
