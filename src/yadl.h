#include<sys/types.h>
#include<stdint.h>
#include "block.h"
#include "hash.h"
#include<unistd.h>
#include<fcntl.h>

/*enum to define yadl errors*/
typedef enum ydl_errors 
{
	ydl_stderr = -1     
}ydl_error;

/*struct to define a config store for yadl*/
struct ydl_store_config
{
    	enum hash_type ht;
    	enum chunk_type ct;
};

/*struct to define a key value pair for yadl*/
struct ydl_key_value
{
    	char *key;
    	void *value;
    	ssize_t value_len;
}; 

/*struct to define a object meta for yadl used as linked list to store key value pair*/
struct ydl_object_meta
{
    	struct ydl_key_value *next;
};

/*struct to define object for yadl*/
struct ydl_object_t
{
    	uint64_t dobject_id;
    	struct ydl_object_meta *obj_meta;
};

/*enum to define modes of opening a file*/
typedef enum ydl_mode
{
	APPEND,
	RDONLY,
	WRONLY,
	RDWR,
	CREAT
}ydl_modes;
 
/*@description:Function to create or open existing ydlobject 
@in: struct ydl_object_meta *-object to store metadata of yadl,enum ydl_mode-different modes to open a file, ydl_error-error handling for errors occured in yadl,int *return_value-return status of the function
@out: ydl_object_t 
@return: */
struct 
ydl_object_t  *ydl_open(struct ydl_object_meta *,enum ydl_mode, ydl_error,int *return_value);

/*@description:Function to write to yadl store.
@in: struct ydl_object_t  *-object to store write information , const char  *buffer-to store contents in yadl store, ssize_t length-length of buffer
@out: ssize_t-number of bytes written
@return: */
ssize_t  
ydl_write (struct ydl_object_t  *, const char  *buffer, ssize_t length); 

/*@description:Function to read from yadl store.
@in: struct ydl_object_t  *-object to store read information, const char  *buffer-to store contents in yadl store, ssize_t length-length of buffer
@out: ssize_t-number of bytes read
@return: */
ssize_t  
ydl_read (struct ydl_object_t  *, const char  *buffer, ssize_t length);

/*@description:Function to read metadata from yadl store.
@in: struct ydl_object_t  *-object to store read metadata information,struct ydl_object_meta *-meta information of object
@out: ydlerror
@return: */
ydl_error 
ydl_read_meta(struct ydl_object_t  *, struct ydl_object_meta *);

/*@description:Function to truncate meta to store. 
@in: struct ydl_object_t  *-object to store information of yadl, void *value-value
@out: ydlerror
@return: */
ydl_error 
ydl_truncate_meta (struct ydl_object_t  *);

/*@description:Function to append meta to store. 
@in: struct ydl_object_t  *-object to store information of yadl, void *value-value
@out: ydlerror
@return: */
ydl_error 
ydl_append_meta (struct ydl_object_t  *,struct ydl_key_value *); 

/*@description:Function to change meta from store. 
@in: struct ydl_object_t  *-object to store information of yadl, const char *key-key, void *value-value, ssize_t value_len-length of value
@out: ydlerror
@return: */
ydl_error 
ydl_change_meta (struct ydl_object_t  *, const char *key, const void *value,ssize_t value_len);

/*@description:Function to delete key value meta from store. 
@in: struct ydl_object_t  *-object to store information of yadl, const char *key-key
@out: ydlerror
@return: */
ydl_error 
ydl_delete_meta (struct ydl_object_t  *, const char *key);

/*@description:Function to find key value meta from store. 
@in: struct ydl_object_t  *-object to store information of yadl, const char *key-key, void *value-value, ssize_t *len-length of value
@out: ydlerror
@return: */
ydl_error 
ydl_find_meta_key_value (struct ydl_object_t  *, const char *key, void *value, ssize_t *len);

/*@description:Function to close existing ydlobject 
@in: struct ydl_object_meta *-object to store information of yadl
@out: ydlerror
@return: */
ydl_error 
ydl_close (struct ydl_object_t  *);
