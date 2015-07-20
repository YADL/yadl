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

/*Function to create the namespace with given arguments.
Input:
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int
create_namespace(char *namespace_path)
{
        int     ret             =       -1;
        int     fd              =       -1;
        char    filename[1024]  =       "";
        char    content[1024]   =       "";
        char    path[1024]      =       "";
        DIR     *dp             =       NULL;
        struct stat st;

        if (namespace_path == NULL || nm.store_path == NULL ||
                nm.store_type == NULL || nm.hash_type == NULL ||
                nm.chunk_scheme == NULL) {
                printf("Invalid argument list\n");
                goto out;
        }

        sprintf(filename, "%s/%s.yadl", namespace_path, nm.namespace_name);
        if (stat (filename, &st) != 0) {

                if (nm.namespace_name == NULL) {
                        printf("Invalid Namespace name\n");
                        goto out;
                }

                dp = opendir(nm.store_path);
                if (dp == NULL) {
                        printf("Invalid store path\n");
                        goto out;
                } else {
                        closedir(dp);
                        sprintf(path, "%s/store_block", nm.store_path);
                        dp = opendir(path);
                        if (dp != NULL) {
                                fprintf(stderr,
                                "\nGiven store path is already occupied by "
                                "other namespace\n\n");
                                goto out;
                        }
                }

                if (strcmp(nm.store_type, "default") != 0 &&
                strcmp(nm.store_type, "object") != 0) {
                        printf("Invalid store type\n");
                        goto out;
                }

                if (strcmp(nm.hash_type, "md5") != 0 &&
                strcmp(nm.hash_type, "sha1") != 0) {
                        printf("Invalid hash type\n");
                        goto out;
                }

                if (strcmp(nm.chunk_scheme, "fixed") != 0 &&
                strcmp(nm.chunk_scheme, "variable") != 0) {
                        printf("Invalid chunk_scheme\n");
                        goto out;
                }

                if (strcmp(nm.chunk_scheme, "fixed") == 0 &&
                nm.chunk_size < 1) {
                        printf("Invalid chunk_size\n");
                        goto out;
                }

                if (strcmp(nm.chunk_scheme, "variable") == 0 &&
                nm.chunk_size > 0) {
                        printf("Variable chunk_scheme does not accept "
                        "chunk_size\n");
                        goto out;
                }
                fd = open(filename, O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
                if (fd < 1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                sprintf(content, "store_path:%s\n", nm.store_path);
                sprintf(content, "%sstore_type:%s\n", content, nm.store_type);
                sprintf(content, "%shash_type:%s\n", content, nm.hash_type);
                sprintf(content, "%schunk_scheme:%s\n", content,
                        nm.chunk_scheme);
                sprintf(content, "%schunk_size:%lu\n", content, nm.chunk_size);
                sprintf(content, "%sdesc:%s\n", content, nm.desc);
                ret = write (fd, content, strlen(content));
                if (ret < 0)
                        goto out;
                printf("namespace created %s\n", filename);
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
Output:
        int : Return 0 on success -1 on failure.
*/
int
namespace_info(char *namespace_path)
{
        int     ret             =       -1;
        int     fd              =       -1;
        char    filename[1024]  =       "";
        char    content[1024]   =       "";
        DIR           *dp       =       NULL;
        struct dirent *dir;

        if (nm.namespace_name == NULL) {
                goto out;
        }
        if (namespace_path == NULL) {
                goto out;
        }

        if (strcmp(nm.namespace_name, "all") != 0) {
                sprintf(filename, "%s/%s.yadl", namespace_path,
                        nm.namespace_name);
                printf("%s :\n", filename);
                fd = open(filename, O_RDONLY, S_IRUSR|S_IWUSR);
                if (fd < 1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                ret = read(fd, content, 1024);
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
                                        ret = read(fd, content, 1024);
                                        if (ret < 0) {
                                                fprintf(stderr, "%s\n",
                                                        strerror(errno));
                                                goto out;
                                        }
                                        printf("\n%s\n", content);
                                        memset(content, 0, 1024);
                                }
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
        int     ret             =       -1;
        char    filename[1024]  =       "";
        DIR           *dp       =       NULL;
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
                        }
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

/*Function to delete the namespace.
Input:
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int
delete_namespace(char *namespace_path)
{

        int     ret                     =       -1;
        char    namespace_file[1024]    =       "";
        char    filename[1024]          =       "";
        DIR           *dp               =       NULL;
        struct dirent *dir;

        if (namespace_path == NULL) {
                goto out;
        }
        if (nm.namespace_name == NULL) {
                goto out;
        }

        sprintf(namespace_file, "%s.yadl", nm.namespace_name);
        dp = opendir(namespace_path);
        if (dp) {
                while ((dir = readdir(dp)) != NULL) {
                        if (strcmp(dir->d_name, ".") != 0 &&
                                strcmp(dir->d_name, "..") != 0) {
                                if (strcmp(dir->d_name, namespace_file) == 0) {
                                        sprintf(filename, "%s/%s",
                                                namespace_path, namespace_file);
                                        ret = file_operation(reset, NULL,
                                                namespace_path);
                                        if (ret == -1 || ret == 1) {
                                                goto out;
                                        }
                                        ret = remove(filename);
                                        if (ret == -1) {
                                                fprintf(stderr, "%s\n",
                                                        strerror(errno));
                                                goto out;
                                        }
                                }
                        }
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
        int ret         =         -1;
        char path[1024] =         "";
        char confirm    =        'n';
        DIR     *dp     =       NULL;

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
                memset(path, 0, 1024);
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

/*Function to call different file operation functions with vaid inputs.
 initiate various stores.
Input:
        enum FLAG flag : Notifies which file operation to be performed.
        char *filename : File that to be operated.
        char *namespace_path : Path of the namespace.
Output:
        int : Return 0 on success -1 on failure.
*/
int
file_operation(enum FLAG flag, char *filename, char *namespace_path)
{
        char    *buffer         =       NULL;
        DIR     *dp             =       NULL;
        int     index           =        0;
        int     ret             =       -1;
        int     fd              =       -1;
        char    namespace_filename[1024];
        char    *token1         =       NULL;
        char    *token2         =       NULL;
        char    *str            =       NULL;
        char    *sub_str        =       NULL;
        char    *key_value[3];
        const char dlmtr1[2]    =       ":";
        const char dlmtr2[2]    =       "\n";
        char    *saveptr1       =       NULL;
        char    *saveptr2       =       NULL;
        char    path[1024]      =       "";

        if (namespace_path == NULL) {
                goto out;
        }

        if (nm.namespace_name == NULL) {
                printf("Namespace not specified");
                goto out;
        }

        sprintf(namespace_filename, "%s/%s.yadl", namespace_path,
                nm.namespace_name);
        fd = open(namespace_filename, O_RDONLY, S_IWUSR|S_IRUSR);
        if (fd < 1) {
                fprintf(stderr, "%s\nNamespace does not exists\n",
                        strerror(errno));
                goto out;
        }
        buffer = (char *)calloc(1, 1024);
        ret = read(fd, buffer, 1024);
        if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

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
                        nm1.store_path = key_value[1];
                        if (nm1.store_path == NULL) {
                                goto out;
                        }
                }
                if (strcmp(key_value[0], "store_type") == 0) {
                        nm1.store_type = key_value[1];
                        if (nm1.store_type == NULL) {
                                goto out;
                        }
                }
                if (strcmp(key_value[0], "hash_type") == 0) {
                        nm1.hash_type = key_value[1];
                        if (nm1.hash_type == NULL) {
                                goto out;
                        }
                }
                if (strcmp(key_value[0], "chunk_scheme") == 0) {
                        nm1.chunk_scheme = key_value[1];
                        if (nm1.chunk_scheme == NULL) {
                                goto out;
                        }
                }
                if (strcmp(key_value[0], "chunk_size") == 0) {
                        nm1.chunk_size = atoi(key_value[1]);
                }
                index = 0;
        }
        sprintf(path, "%s/store_block", nm1.store_path);
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
        if (flag == dedup) {
                if (filename == NULL) {
                        printf("File name requried :"
                        "Try $>yadl --help for more information\n");
                        goto out;
                }
                ret = dedup_file(nm1, filename);
                if (ret < 0)
                        goto out;
        }
        if (flag == restore) {
                if (filename == NULL) {
                        printf("File name requried :"
                        "Try $>yadl --help for more information\n");
                        goto out;
                }
                ret = restore_file(filename, nm1.store_path);
                if (ret < 0)
                        goto out;
        }
        if (flag == delete_file) {
                if (filename == NULL) {
                        printf("File name requried :"
                        "Try $>yadl --help for more information\n");
                        goto out;
                }
                ret = delete_stub_store(nm1.store_path, filename);
                if (ret < 0)
                        goto out;
                printf("\nStub deleted\n");
        }
        if (flag == list) {
                ret = readfilecatalog();
                if (ret < 0)
                        goto out;
        }
        if (flag == reset) {
                ret = clear_store(nm1.store_path);
                if (ret < 0)
                        goto out;
                dp = opendir(path);
                if (NULL != dp) {
                        ret = 1;
                        goto out;
                }
                ret = mkdir(path, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
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
        enum    FLAG flag               =       -1;
        int     option_index            =        0;

        const char *short_options = "cn:p:h:s:df:ilRr";

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
                {"reset",            no_argument,            0,   'R'},
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
                                nm.chunk_scheme = optarg;
                        }
                        if (strcmp(long_options[option_index].name,
                        "chunk_size") == 0) {
                                nm.chunk_size = atoi(optarg);
                        }
                        if (strcmp(long_options[option_index].name,
                        "desc") == 0) {
                                nm.desc = optarg;
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
                        nm.namespace_name = optarg;
                        break;

                case 'p':
                        nm.store_path = optarg;
                        break;

                case 's':
                        nm.store_type = optarg;
                        break;

                case 'h':
                        nm.hash_type = optarg;
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
        case 0:
                ret = create_namespace(namespace_path);
                if (ret == -1)
                        goto out;
                break;
        case 1:
                ret = create_namespace(namespace_path);
                if (ret == -1)
                        goto out;
                break;
        case 2:
                if (file_path == NULL) {
                        ret = delete_namespace(namespace_path);
                        if (ret < 0) {
                                goto out;
                        }
                        break;
                }
        case 3:
        case 4:
        case 6:
                if (nm.namespace_name == NULL) {
                        ret = list_namespace(argc, namespace_path);
                        if (ret == -1)
                                goto out;
                        break;
                }
        case 7:
                ret = file_operation(flag, file_path, namespace_path);
                if (ret == -1)
                        goto out;
                break;
        case 5:
                ret = namespace_info(namespace_path);
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
