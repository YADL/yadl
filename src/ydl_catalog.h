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
#include "clean_buff.h"
#define INT_SIZE sizeof(int)
#ifndef YDL_CATALOG_H
#define YDL_CATALOG_H

class ydl_catalog
{
        static int cat_fd;
public:
        int init_catalog_store();
        int writecatalog(char* filename);
        int readfilecatalog();
        int comparepath(char *out);
        int fini_catalog_store();
};

int ydl_catalog::cat_fd;
#endif // YDL_CATALOG_H
