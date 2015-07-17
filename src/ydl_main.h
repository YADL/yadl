#include<stdio.h>
#include<libgen.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<fcntl.h>
#include "ydl_catalog.h"
#include "ydl_block.h"
#include "ydl_hash.h"
#include "ydl_dedup.h"
#include "ydl_restore.h"
#include "ydl_delete.h"

#define FILE_SIZE  200

enum stores { default_store = 0, object_store };

inline int file_exist(char *filename);