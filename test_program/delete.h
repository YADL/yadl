#include "clean_buff.h"
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include<cmockery/pbc.h>
#include <cmockery/cmockery.h>
#define FILE_SIZE  200

/*@description:Function to read the full path of file from catalog
@in: void
@out: int 
@return: -1 for error and 0 if read successfully */
int readfilecatalog();

/*@description:Function to delete file.
@in: void
@out: int 
@return: -1 for error and 0 if read successfully */
int deletefile();
