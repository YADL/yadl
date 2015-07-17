#include <iostream>
using namespace std;
#include<stdio.h>
#include<string.h>  
#include<stdlib.h> 
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<ctype.h> 
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include <libgen.h>
#include "ydl_block.h"
#include "ydl_catalog.h"
#include "ydl_hash.h"
#include "ydl_object_store.h"

#define NAME_SIZE 100
#define int_size sizeof(int)
#define FILE_SIZE  200

class ydl_restore
{
public:
        int restorefile(char* path);
        int restore_file();
};