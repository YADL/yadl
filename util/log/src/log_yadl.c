#include "log_yadl.h"

int
log_init(char *path,char *dir)
{
        int     ret    =    -1;
        int     pid    =    -1;

        char    log_name[100]    =    "";
        char    dir_path[100]    =    "";

        struct stat st;

        assert(path);
        assert(dir);

        sprintf(dir_path,"%s%s",path,dir);

        ret=mkdir( dir_path , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

        if( ret != 0 && !( stat ( dir_path , &st ) == 0 &&
                S_ISDIR ( st.st_mode )))
        {
                printf("Error creating the directory: %s\n",strerror(errno));
                goto out;
        }

        ret=chdir(dir_path);
        if(ret!=0)
        {
                printf("Error in changing directory: %s\n",strerror(errno));
                goto out;
        }

        pid=getpid();
        sprintf(log_name,"%s%d.log",dir,pid);

        fp=fopen(log_name,"w+");
        if(fp==NULL)
        {
                printf("Error creating log file %s\n",log_name);
                goto out;
        }

        ret=0;
out:
        return ret;
}

void
log_fin()
{
        fclose(fp);
}

void
log_write(int log_lev,char *comp,char *frmt,...)
{
        va_list args;

        if( (LOG_MODE==0 && (log_lev==0 || log_lev==1)) || (LOG_MODE==1 &&
                (log_lev==2 || log_lev==3)) )
        {
                if(comp!=NULL)
                        fprintf(fp,"Component: %s\n",comp);

                va_start(args,frmt);
                vfprintf(fp,frmt,args);
                va_end(args);
        }
}