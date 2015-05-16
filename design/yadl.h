typedef enum ydlerrors 
{
	ydl_stderr = -1;     
}ydl_error_t;

enum hash_type
{
	sha1    =   1,
	sha256,
	md5
};

enum chunk_type
{
	fixed_chunk     =   1,
    	variable_chunk
};

struct ydl_store_config
{
    	enum hash_type;
    	enum chunk_type;
};

struct ydlkey_value
{
    	char *key;
    	void *value;
    	ssize_t value_len;
}; 

struct ydlobject_meta
{
    	struct ydlkey_value *next;
};

struct ydlobject
{
    	uint64/uuid dobject_id;
    	ydlobject_meta *;
};

enum ydl_mode
{
	O_APPEND = 1.
	O_RDONLY,
	O_WRONLY
};
 
/*@description:Function to create or open existing ydlobject 
@in: struct ydlobject_meta *-object to store metadata of yadl,enum ydl_mode-different modes to open a file, ydl_error_t-error handling for errors occured in yadl,int *return_value-return status of the function
@out: ydlobject 
@return: */
ydlobject  
*ydlopen(struct ydlobject_meta *,enum ydl_mode, ydl_error_t,int *return_value);

/*@description:Function to write to yadl store.
@in: struct ydlobject  *-object to store write information , const char  *buffer-to store contents in yadl store, ssize_t length-length of buffer
@out: ssize_t-number of bytes written
@return: */
ssize_t  
ydlwrite (struct ydlobject  *, const char  *buffer, ssize_t length); 

/*@description:Function to read from yadl store.
@in: struct ydlobject  *-object to store read information, const char  *buffer-to store contents in yadl store, ssize_t length-length of buffer
@out: ssize_t-number of bytes read
@return: */
ssize_t  
ydlread (struct ydlobject  *, const char  *buffer, ssize_t length);

/*@description:Function to read metadata from yadl store.
@in: struct ydlobject  *-object to store read metadata information,struct ydlobject_meta *-meta information of object
@out: ydlerror
@return: */
ydlerror 
ydlread_meta(struct ydlobject  *, struct ydlobject_meta *);

/*@description:Function to truncate meta to store. 
@in: struct ydlobject  *-object to store information of yadl, void *value-value
@out: ydlerror
@return: */
ydlerror 
ydtruncate_meta (struct ydlobject  *);

/*@description:Function to append meta to store. 
@in: struct ydlobject  *-object to store information of yadl, void *value-value
@out: ydlerror
@return: */
ydlerror 
ydlappend_meta (struct ydlobject  *, ydlkey_value *); 

/*@description:Function to change meta from store. 
@in: struct ydlobject  *-object to store information of yadl, const char *key-key, void *value-value, ssize_t *len-length of value
@out: ydlerror
@return: */
ydlerror 
ydlchange_meta (struct ydlobject  *, const char *key, const void *value, value_len);

/*@description:Function to delete key value meta from store. 
@in: struct ydlobject  *-object to store information of yadl, const char *key-key
@out: ydlerror
@return: */
ydlerror 
ydldelete_meta (struct ydlobject  *, const char *key);

/*@description:Function to find key value meta from store. 
@in: struct ydlobject  *-object to store information of yadl, const char *key-key, void *value-value, ssize_t *len-length of value
@out: ydlerror
@return: */
ydlerror 
ydlfind_meta_key_value (struct ydlobject  *, const char *key, void *value, ssize_t *len);

/*@description:Function to close existing ydlobject 
@in: struct ydlobject_meta *-object to store information of yadl
@out: ydlerror
@return: */
ydlerror 
ydlclose (struct ydlobject  *);
