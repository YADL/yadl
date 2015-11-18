#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <getopt.h>
#include <leveldb/c.h>
#include <fcntl.h>
#include <error.h>

/* Function to restore it with original contents.
Input   :  char* path
Output  :  int
*/
int minhash_restore(char *path);