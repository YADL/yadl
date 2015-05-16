#include "yadl.h"

ydlobject  
*ydlopen(ydlobject_meta *, ydl_mode, enum ydlerror,int *return_value) 
{

	return NULL;

}

ssize_t  
ydlwrite (ydlobject  *, const char  *buffer, ssize_t length) 	
{

	return NULL:

}	
 
ssize_t  
ydlread (ydlobject  *, const char  *buffer, ssize_t length) 
{

	return NULL:

}
	  
ydlerror 
ydlread_meta (ydlobject  *, ydlobject_meta *) 
{

	return NULL;
	
} 
 
ydlerror 
ydltruncate_meta (ydlobject  *) 
{

	return NULL:

} 
 
ydlerror 
ydlappend_meta (ydlobject  *, ydlkey_value *) 
{

	return NULL:

} 
 
ydlerror 
ydlchange_meta (ydlobject  *, const char *key, const void *value, value_len)
{

	return NULL:;

} 
 
ydlerror 
ydldelete_meta (ydlobject  *, const char *key)
{

	return NULL:

} 
 
ydlerror 
ydlfind_meta_key_value (ydlobject  *, const char *key, void *value, ssize_t *len)
{

	return NULL:

}
 
ydlerror 
ydlclose (ydlobject  *)
{

	return NULL:

}
