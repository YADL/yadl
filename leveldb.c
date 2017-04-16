#include <leveldb/c.h>
#include <stdio.h>
int main() 
{
	leveldb_t 		*db		;
	leveldb_options_t 	*options	;
	leveldb_readoptions_t 	*roptions	;
	leveldb_writeoptions_t 	*woptions	;
	char *error 	= 	NULL		;
	char *value				;
	size_t value_len			;
	int 	ret	=	-1		;

	/* create or opening the database */
	options = leveldb_options_create();
	leveldb_options_set_create_if_missing(options, 1);
	db = leveldb_open(options, "./test_leveldb", &error);
	if (error != NULL) 
	{
		fprintf(stderr, "Openinig of database fail.\n");
		ret	=	-1;
		goto out;
	}
	leveldb_free(error); 
	error = NULL;	
	/* Write to the database */
	woptions = leveldb_writeoptions_create();
	leveldb_put(db, woptions, "Name", 4, "Srinivas", 8, &error);
	if (error != NULL) 
	{
		fprintf(stderr, "Writing to database failed.\n");
		ret 	= 	-1;
		goto out;
	}
	leveldb_free(error); 
	error = NULL;
	/* Read from the database */
	roptions = leveldb_readoptions_create();
	value = leveldb_get(db, roptions, "Name", 4, &value_len, &error);
	if (error!= NULL) 
	{
		fprintf(stderr, "Reading from database is failed.\n");
		ret	=	-1;
		goto out;
	}
	printf("Name: %s \n",value);
	printf("length     %zu   \n",value_len);
	leveldb_free(error); 
	error = NULL;
	/* Delete key from database */
	leveldb_delete(db, woptions, "Name", 4, &error);
	if (error != NULL) 
	{
		fprintf(stderr, "Reading from database is failed.\n");
		ret	=	-1;
		goto out;
	}
	leveldb_free(error); 
	error = NULL;
	/* Closing the database */
	leveldb_close(db);
	leveldb_free(error); 
	error = NULL;
	ret	=	0;
out:
	return	ret;
}
