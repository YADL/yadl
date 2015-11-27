#include "ydl_namespace.h"
#include "ydl_clean_buff.h"

int create_namespace(char *ns_name, char *ns_desc, char *ns_hash_type,
                                char *ns_store_type, char *ns_chunk_scheme,
                                char *ns_chunk_size) {

//printf("%s\t%s\t%s\t%s\t%s\t%s\n",ns_name,ns_desc,ns_hash_type,ns_store_type,ns_chunk_scheme,ns_chunk_size);

        leveldb_t *db                     =       NULL;
        leveldb_options_t *option         =       NULL;
        leveldb_writeoptions_t *write_opt =       NULL;

        char *ret_value                   =       NULL;
        char namespace_path[50]           =         "";
        int ret                           =         -1;
        DIR     *dp                       =       NULL;

        option = leveldb_options_create();
        leveldb_options_set_create_if_missing(option, 1);
        dp = opendir(NAMESPACE_PATH);
        if (NULL == dp) {
                ret = mkdir(NAMESPACE_PATH, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }

        sprintf(namespace_path,"%s%s.yadl",NAMESPACE_PATH,ns_name);
        db = leveldb_open(option, namespace_path, &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Error in opening\n");
                goto out;
        }
        clean_buff(&ret_value);
        write_opt = leveldb_writeoptions_create();
        ret = write_to_namespace(db, write_opt, "store_type", ns_store_type);
        if(ret == -1)
                goto out;

        ret = write_to_namespace(db, write_opt, "hash_type", ns_hash_type);
        if(ret == -1)
                goto out;

        ret = write_to_namespace(db, write_opt, "chunk_scheme", ns_chunk_scheme);
        if(ret == -1)
                goto out;

        ret = write_to_namespace(db, write_opt, "chunk_size", ns_chunk_size);
        if(ret == -1)
                goto out;

        ret = write_to_namespace(db, write_opt, "desc", ns_desc);
        if(ret == -1)
                goto out;
        printf("Namespace created\n");

out:
        leveldb_close(db);
        return ret;

}

int write_to_namespace(leveldb_t *db, leveldb_writeoptions_t *write_opt,
                        char *key, char *value) {

        char *ret_value                 =       NULL;
        int ret                         =         -1;
//printf("Key: %s\nValue: %s\n",key,value);
        leveldb_put(db, write_opt, key, strlen(key), value, strlen(value),
                        &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Write fail.\n");
                goto out;
        }
        clean_buff(&ret_value);
        ret = 0;

out:

        return ret;

}

namespace* open_namespace(char *ns_name) {

        DIR *directory                  =       NULL;
        struct dirent* dir_entry        =       NULL;
        leveldb_t *db                   =       NULL;
        leveldb_options_t *option       =       NULL;
        leveldb_readoptions_t *read_opt =       NULL;
        char *ret_value                 =       NULL;
        namespace *ns                   =       NULL;
        char *read                      =       NULL;
        char name[50]                   =         "";
        char namespace_path[50]         =         "";
        int ret                         =         -1;
        struct stat st;

        directory = opendir(NAMESPACE_PATH);
        if(NULL == directory) {
                fprintf(stderr, "%s\n",strerror(errno));
                goto out;
        }
        sprintf(namespace_path,"%s/%s.yadl", NAMESPACE_PATH, ns_name);
        /*sprintf(name,"%s.yadl",ns_name);
        while(1) {
                        dir_entry = readdir(directory);
                        if(dir_entry != NULL)
                        {*/
                        if(stat(namespace_path, &st) < 0) {
                                fprintf(stderr, "Namespace does not exist\n");
                                goto out;
                        }
                                /*if(strcmp(name,dir_entry->d_name) == 0) {*/
                                option = leveldb_options_create();
                                leveldb_options_set_create_if_missing(option, 1);
                                db = leveldb_open(option, namespace_path, &ret_value);
                                if (ret_value != NULL) {
                                        fprintf(stderr, "Error in opening\n");
                                        goto out;
                                }
                                clean_buff(&ret_value);
                                read_opt = leveldb_readoptions_create();
                                ns = (namespace*)malloc(sizeof(namespace));

                                read = read_from_namespace(db, read_opt, "hash_type");
                                if (read == NULL)
                                        goto out;
                                ns->hash_type = (char*)malloc(strlen(read));
                                strcpy(ns->hash_type, read);
                                clean_buff(&read);

                                read = read_from_namespace(db, read_opt, "store_type");
                                if (read == NULL)
                                        goto out;
                                ns->store_type = (char*)malloc(strlen(read));
                                strcpy(ns->store_type, read);
                                clean_buff(&read);

                                read = read_from_namespace(db, read_opt, "chunk_scheme");
                                if (read == NULL)
                                        goto out;
                                ns->chunk_scheme = (char*)malloc(strlen(read));
                                strcpy(ns->chunk_scheme, read);
                                clean_buff(&read);

                                read = read_from_namespace(db, read_opt, "chunk_size");
                                if (read == NULL)
                                        goto out;
                                ns->chunk_size = atoi(read);
                                clean_buff(&read);

                                read = read_from_namespace(db, read_opt, "desc");
                                if (read == NULL)
                                        goto out;
                                ns->desc = (char*)malloc(strlen(read));
                                strcpy(ns->desc,read);
                                clean_buff(&read);
                                ret = 0;
                                //goto out;
                        /*}
                }
        }*/

out:
        leveldb_close(db);
        return ns;
}

char *read_from_namespace(leveldb_t *db, leveldb_readoptions_t *read_opt, char *key) {

        char *ret_value                 =       NULL;
        char *read                      =       NULL;
        size_t read_len                 =          0;

        read = leveldb_get(db, read_opt, key, strlen(key), &read_len, &ret_value);
        if (ret_value != NULL) {
                fprintf(stderr, "Read fail.\n");
                goto out;
        }
        printf("READ: %d\t%s\n",strlen(read),read);
        clean_buff(&ret_value);
out:
        return read;

}
