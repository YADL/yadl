#include <stdio.h>
#include <string.h>
#include "ydl_clean_buff.h"
#include "ydl_namespace.h"
int main() {
        int ret=-1;
        namespace *ns = NULL;
        char name[10],desc[20],hash[10],store[10],scheme[10],path[50];
        int size;
        printf("name,desc,md5,default,variable,0,store path\n");
        ret = create_namespace("ns4","Namespace ns3","md5","object","/home/kus/storepath4","fixed",1024);
        printf("%d\n",ret);
        return ret;
}