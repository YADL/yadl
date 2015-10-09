#include <stdio.h>
#include <string.h>
#include "ydl_clean_buff.h"
#include "ydl_namespace.h"
int main() {
        int ret=-1;
        namespace *ns = NULL;
        char name[10],desc[20],hash[10],store[10],scheme[10],size[10];
        printf("name,desc,md5,default,variable,0\n");
        scanf("%s",&name);
        scanf("%s",&desc);
        scanf("%s",&hash);
        scanf("%s",&store);
        scanf("%s",&scheme);
        scanf("%s",&size);
        ret = create_namespace(name,desc,hash,store,scheme,size);
        /*ret = create_namespace("ns115","ns","sha1","object","variable","0");
        ns = open_namespace("ns115");*/
        ns = open_namespace(name);
        printf("Hash Type: %s\nStore Type: %s\nChunk Scheme: %s\nChunk Size: %d\nDescription: %s\n",ns->hash_type,ns->store_type,ns->chunk_scheme,ns->chunk_size,ns->desc);
        return 0;
}