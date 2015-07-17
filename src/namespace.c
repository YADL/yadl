#include "namespace.h"
#include "block.h"
#include "catalog.h"
#include "hash.h"
#include "restore.h"
#include "stub.h"

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
                " --desc           Description of namespace\n\n"
                "$> yadl --create/-c  -n <namespace_name> --store_path <store_path>\n"
                                          "[--hash_type {md5/sha1}]\n"
                                          "[--chuck_scheme {variable/fixed} [--chuck_size <chuck_size>]  ]\n"
                                          "[--store_type {default/object}] [--desc <namespace_description>]\n");
}

int
create_namespace(int arg_count, char *namespace_path)
{
        int     ret             =       -1;
        int     fd              =       -1;
        char    filename[1024]  =       "";
        char    content[1024]   =       "";
        struct stat st;

        sprintf(filename, "%s/%s.yadl", namespace_path, nm.namespace_name);
        if (stat (filename, &st) != 0) {
                fd = open(filename, O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
                if( fd < 1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                sprintf(content, "store_path:%s\n", nm.store_path);
                sprintf(content, "%sstore_type:%s\n", content, nm.store_type);
                sprintf(content, "%shash_type:%s\n", content, nm.hash_type);
                sprintf(content, "%schunk_scheme:%s\n", content, nm.chunk_scheme);
                sprintf(content, "%schunk_size:%d\n", content, nm.chunk_size);
                sprintf(content, "%sdesc:%s\n", content, nm.desc);
                write (fd, content, strlen(content));
        } else {
                printf("namespace already present\n");
        }
        ret = 0;
out :
        return ret;
}

int
file_operation(enum FLAG flag, char *filename, char *namespace_path)
{
        char    *buffer         =       NULL;
        DIR     *dp             =       NULL;
        int     i               =        0;
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

        sprintf(namespace_filename, "%s/%s.yadl", namespace_path,
                dedup_struct.namespace_name);
        fd = open(namespace_filename, O_RDONLY, S_IWUSR|S_IRUSR);
        if (fd < 1)
        {
                fprintf(stderr, "%s\nNamespace does not exists\n",
                        strerror(errno));
                goto out;
        }
        buffer = (char *)calloc(1, 1024);
        ret = read(fd, buffer, 1024);
        if (ret < 0)
        {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }

        for (str = buffer; ;str = NULL) {
                token1 = strtok_r(str, dlmtr2, &saveptr1);
                if (token1 == NULL)
                    break;

               for (sub_str = token1; ; sub_str = NULL) {
                        token2 = strtok_r(sub_str, dlmtr1, &saveptr2);
                        if (token2 == NULL)
                                break;
                        key_value[i] = token2;
                        i++;
                }
                if (strcmp(key_value[0],"store_path") == 0) {
                        nm1.store_path = key_value[1];
                }
                if (strcmp(key_value[0],"store_type") == 0) {
                        nm1.store_type = key_value[1];
                }
                if (strcmp(key_value[0],"hash_type") == 0) {
                        nm1.hash_type = key_value[1];
                }
                if (strcmp(key_value[0],"chunk_scheme") == 0) {
                        nm1.chunk_scheme = key_value[1];
                }
                if (strcmp(key_value[0],"chunk_size") == 0) {
                        nm1.chunk_size = atoi(key_value[1]);
                }
                i = 0;
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
                ret = dedup_file(nm1,filename);
                if (ret < 0)
                        goto out;
        }
        if (flag == restore) {
                ret = restore_file(filename, nm1.store_path);
                if (ret < 0)
                        goto out;
        }
        if (flag == delete_file) {
                ret = delete_stub_store(nm1.store_path, filename);
                if (ret < 0)
                        goto out;
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
out :
        if(dp != NULL)
                closedir(dp);
        return ret;
}

int
start_program(int argc, char **argv, char *namespace_path)
{

        int     choice                  =       -1;
        int     ret                     =       -1;
        char    *file_path              =     NULL;
        enum    FLAG flag               =       -1;
        int     option_index            =        0;
        
        const char *short_options = "cn:p:h:s:df:";

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
                {"delete",         no_argument,            0,   'd'},
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
                        if (strcmp(long_options[option_index].name,"chunk_scheme") == 0) {
                                nm.chunk_scheme = optarg;
                        }
                        if (strcmp(long_options[option_index].name,"chunk_size") == 0) {
                                nm.chunk_size = atoi(optarg);
                        }
                        if (strcmp(long_options[option_index].name,"desc") == 0) {
                                nm.desc = optarg;
                        }
                        if (strcmp(long_options[option_index].name,"dedup") == 0) {
                                flag = dedup;
                        }
                        break;

                case 'c':
                        flag = create;
                        break;

                case 'n':
                        nm.namespace_name = optarg;
                        dedup_struct.namespace_name = optarg;
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
                        flag = delete_file;
                        break;

                case 'r':
                        flag = restore;
                        break;

                case 'f' :
                        file_path = optarg;
                        break;

                case '?':
                        print_usage (stderr, 1);
                        break;

                default:
                        printf("?? getopt returned character code 0%o ??\n", choice);
                }
        }

        if (optind < argc) {
                printf("non-option ARGV-elements: ");
                while (optind < argc)
                        printf("%s \n", argv[optind++]);
                print_usage(stderr, 1);
                goto out;
        }
        switch(flag) {
        case 0 :
                ret = create_namespace(argc, namespace_path);
                if(ret == -1)
                        goto out;
                break;
        case 1 :
                ret = create_namespace(argc, namespace_path);
                if(ret == -1)
                        goto out;
                break;
        case 2 :
        case 3 :
        case 4 :
                ret = file_operation(flag,file_path, namespace_path);
                if(ret == -1)
                        goto out;
                break;
        }
        ret = 0;
out :
        return ret;
}
