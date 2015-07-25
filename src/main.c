#include "main.h"
#include "string.h"
#include "namespace.h"
#include "catalog.h"
#include "block.h"
#include "hash.h"
#include "dedup.h"
#include "restore.h"
#include "delete.h"
#include "string.h"

/*Main program!*/
int
main (int argc, char *argv[])
{

        int     ret                       = -1;
        char    namespace_path[FILE_SIZE] = "";
        DIR     *dp                       = NULL;

        sprintf(namespace_path, "/var/lib/yadl");
        dp = opendir(namespace_path);
        if (NULL == dp) {
                ret = mkdir(namespace_path, 0777);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
                ret = create_default_namespace(namespace_path);
                if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        goto out;
                }
        }
        ret = start_program(argc, argv, namespace_path);
        if (ret == -1) {
                goto out;
        }
        ret = 0;
out:
        if (dp != NULL)
                closedir(dp);
        return ret;

}
