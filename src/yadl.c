#include "yadl.h"

struct 
ydl_object_t *ydl_open(struct ydl_object_meta *obj_meta,ydl_modes modes, ydl_error error,int *return_value) 
{
	return NULL;	
}

ssize_t  
ydl_write (struct ydl_object_t  *obj, const char  *buffer, ssize_t length) 	
{
	return 0;	
}	
 
ssize_t  
ydl_read (struct ydl_object_t  *obj, const char  *buffer, ssize_t length) 
{
	return 0;
}
	  
ydl_error 
ydl_read_meta (struct ydl_object_t  *obj,struct ydl_object_meta *obj_meta) 
{
	ydl_error error=1;
	return error;	
} 
 
ydl_error 
ydl_truncate_meta (struct ydl_object_t  *obj) 
{
	ydl_error error=1;
	return error;
} 
 
ydl_error 
ydl_append_meta (struct ydl_object_t  *obj,struct ydl_key_value *key_value) 
{
	ydl_error error=1;
	return error;	
} 
 
ydl_error 
ydl_change_meta (struct ydl_object_t  *obj, const char *key, const void *value,ssize_t value_len)
{
	ydl_error error=1;
	return error;
} 
 
ydl_error 
ydl_delete_meta (struct ydl_object_t  *obj, const char *key)
{
	ydl_error error=1;
	return error;
} 
 
ydl_error 
ydl_find_meta_key_value (struct ydl_object_t  *obj, const char *key, void *value, ssize_t *len)
{
	ydl_error error=1;
	return error;
}
 
ydl_error 
ydl_close (struct ydl_object_t  *obj)
{
	ydl_error error=1;
	return error;
}



