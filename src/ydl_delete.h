#include <iostream>
using namespace std;
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include "clean_buff.h"
#define FILE_SIZE  200

class ydl_delete
{
public:
        int readfilecatalog();
        int delete_file();
};