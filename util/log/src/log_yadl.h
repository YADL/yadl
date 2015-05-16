#include<stdio.h>
#include<errno.h>
#include<assert.h>
#include<stdlib.h>
#include<stdarg.h>
#include<pthread.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

#ifdef LOG_MODE
#define LOG_MODE 1
#else
#define LOG_MODE 0
#endif

enum LOG_LEVEL{LOG_INFO=0,LOG_ERROR,LOG_DEBUG,LOG_WARNING};

pthread_mutex_t mutex;

FILE *fp;

/* Creates a directory for the log files under the path specified
 * and initializes the log files.
 * Input:
 *       char *path: Path under which the directory of log files to be created.
 *       char *dir : Name of the directory which contains the log files.
 * Return:
 *       Return 0 if the log file is created under the given path and directory.
 *              -1 otherwise.
 */
int
log_init(char *path,char *dirname);

/*Prints the supplied information on the log file if the mode of logging and
 * the log level satisfice.
 * Input:
 *       int log_lev: Log level of the particulat message.
 *       char *comp : Component name which generated the message.
 *       char *frmt : The format of the input message.
 *       ...        : Used for variable number of arguments.
 */
void
log_write(int log_lev,char *comp,char *format,...);

/* Closes the log file.
 * Input:
 *       void
 * Return:
 *       void
 */
void
log_fini();

