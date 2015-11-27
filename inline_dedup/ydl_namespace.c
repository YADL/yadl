#include "ydl_namespace.h"
#include "ydl_clean_buff.h"

void
print_usage() {

        fprintf(stderr, "The arguments to the create_namespace function are:\n\n");
        fprintf(stderr, "ARGUMENT\tTYPE\tDESCRIPTION\n\n");
        fprintf(stderr, "name\t\tchar*\tName of the namespace\n");
        fprintf(stderr, "desc\t\tchar*\tshort description about the namespace\n");
        fprintf(stderr, "hash type\tchar*\tName of the hashing function (md5/sha1)\n");
        fprintf(stderr, "store type\tchar*\tStore type (default/object)\n");
        fprintf(stderr, "store path\tchar*\tPath where the stores should be saved\n");
        fprintf(stderr, "chunk scheme\tchar*\tChunking scheme (fixed/variable)\n");
        fprintf(stderr, "chunk size\tint\tIf the chunk scheme is variable the");
        fprintf(stderr, "chunk size should be 0 else it should be greater than 0");
}

int
create_namespace(char *name, char *desc, char *hash_type, char *store_type,
                 char *store_path, char *chunk_scheme, int chunk_size) {

        int ret = -1;
        char filename[100] = "";
        char filepath[100] = "";
        struct dirent *d = NULL;
        DIR *dir = NULL;
        struct stat st;

        if(name == NULL || desc == NULL || hash_type == NULL ||
           store_type == NULL || store_path == NULL || chunk_scheme == NULL ||
           chunk_size < 0) {

                fprintf(stderr, "Invalid arguments\n");
                print_usage();
                goto out;
        }

        dir = opendir(NAMESPACE_PATH);
        if (dir == NULL) {
                ret = mkdir(NAMESPACE_PATH,0777);
                if(ret == -1) {
                        fprintf(stderr, "%s\n",strerror(errno));
                        goto out;
                }
                dir = opendir(NAMESPACE_PATH);
                if (dir == NULL) {
                        fprintf(stderr, "%s\n",strerror(errno));
                        goto out;
                }
        }
        sprintf(filepath,"%s/default.yadl",NAMESPACE_PATH);
        if(stat(filepath, &st) < 0) {
                ret = create_namespace_file("default.yadl","Defaultnamespace",
                                            "md5","default","/var/lib/",
                                            "variable",0);
                if(ret == -1){
                        fprintf(stderr, "Error in default namespace creation\n");
                        goto out;
                }
        }
        sprintf(filepath,"%s%s.yadl",NAMESPACE_PATH,name);
        if(stat(filepath, &st) == 0) {
                fprintf(stderr, "Namespace already exists\n");
                goto out;
        }
        sprintf(filename, "%s.yadl", name);
        ret = create_namespace_file(filename, desc, hash_type, store_type,
                                    store_path, chunk_scheme, chunk_size);
        if(ret == -1) {
                fprintf(stderr, "Error in namespace creation\n");
                goto out;
        }
        ret = 0;

out:
        if(dir != NULL)
                closedir(dir);
        return ret;
}

int
create_namespace_file(char *filename, char *desc, char *hash_type,
                      char *store_type, char *store_path, char *chunk_scheme,
                      int chunk_size) {

        char file_name[100] = "";
        DIR *dp = NULL;
        DIR *store_dp = NULL;
        char content[LENGTH] = "";
        char storepath[100] = "";
        int fd = -1;
        int ret = -1;

        dp = opendir(store_path);
        if (dp == NULL) {
                fprintf(stderr, "Invalid store path\n");
                goto out;
        } else {
                closedir(dp);
                sprintf(storepath, "%s/store_block", store_path);
                printf("PATH: %s\n",storepath);
                dp = opendir(storepath);
                if (dp != NULL) {
                        fprintf(stderr,"\nGiven store path is already occupied by some other namespace\n\n");
                        goto out;
                }
        }

        if (strcmp(store_type, "default") != 0 &&
        strcmp(store_type, "object") != 0) {
                fprintf(stderr, "Invalid store type\n");
                goto out;
        }

        if (strcmp(hash_type, "md5") != 0 &&
        strcmp(hash_type, "sha1") != 0) {
                fprintf(stderr, "Invalid hash type\n");
                goto out;
        }

        if (strcmp(chunk_scheme, "fixed") != 0 &&
        strcmp(chunk_scheme, "variable") != 0) {
                fprintf(stderr, "Invalid chunk_scheme\n");
                goto out;
        }

        if (strcmp(chunk_scheme, "fixed") == 0 &&
        chunk_size < 1) {
                fprintf(stderr, "Invalid chunk_size\n");
                goto out;
        }

        if (strcmp(chunk_scheme, "variable") == 0 && chunk_size > 0) {
                fprintf(stderr, "Variable chunk_scheme does not accept chunk_size\n");
                goto out;
        }

        sprintf(file_name,"%s%s", NAMESPACE_PATH, filename);
        fd = open(file_name, O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd < 1) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        sprintf(content, "store_path:%s\n", store_path);
        sprintf(content, "%sstore_type:%s\n", content, store_type);
        sprintf(content, "%shash_type:%s\n", content, hash_type);
        sprintf(content, "%schunk_scheme:%s\n", content, chunk_scheme);
        sprintf(content, "%schunk_size:%lu\n", content, chunk_size);
        sprintf(content, "%sdesc:%s\n", content, desc);
        ret = write (fd, content, strlen(content));
        if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                goto out;
        }
        printf("Namespace %s created\n",filename);
        store_dp = opendir(storepath);
        if (store_dp == NULL) {
                ret = mkdir(storepath, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        printf("Store is created at %s\n", store_path);
        ret = 0;

out:
        return ret;
}
