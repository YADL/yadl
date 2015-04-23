#include "dedup_test.h"
int
dedup_file (char* filename,int chunk_type,int hash_type,int block_size);
extern int writecatalog(char* filename);
int writecatalog(char* filename)
{
	return ((int )mock());
}


// Test case that fails as leak_memory() leaks a dynamically allocated block.
static void leak_memory_test(void **state) 
{
	char 	filename[100];
	int 	fd_input=-1;
	int 	length =1000;
	struct stat st;
	int size;
	printf("enter  filename with fullpath \n");
	scanf("%s",filename);
	fd_input =open(filename,O_APPEND|O_CREAT|O_RDWR);
        if (fd_input< 1)
        {
                fprintf(stderr,"%s\n",strerror(errno));
        }
	fstat(fd_input, &st);
        size = st.st_size;
	while(size>0)
	{
		will_return(searchhash, 1);
		will_return(searchstubhash, 1);
		will_return(write_to_stub, 0);
		will_return(insert_block, 0);
		will_return(insert_hash, 0);
		size=size-length;
		
	}

	will_return(writecatalog, 0);
	assert_int_equal(dedup_file (filename,1,2,length), 0);
	close(fd_input);
	
}
int main(int argc, char* argv[]) 
{

	const UnitTest tests[] = {
        unit_test(leak_memory_test),
        };
    	return run_tests(tests, "dedup_module_test");
}


