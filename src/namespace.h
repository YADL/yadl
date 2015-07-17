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

enum FLAG { create, edit, delete_file, dedup, restore };

struct namespace_dtl
{
        char *namespace_name;
        char *store_path;
        char *desc;
        char *hash_type;
        char *store_type;
        char *chunk_scheme;
        int  chunk_size;
}nm,nm1;

typedef struct namespace_dtl namespace_dtl;

struct dedup_dtl
{
        char *namespace_name;
        char *file_path;
}dedup_struct;

typedef struct dedup_dtl dedup_dtl;

int start_program(int argc, char **argv, char *namespace_path);

int dedup_file (namespace_dtl namespace_input,char *file_path);

