#include <sys/mman.h>
#include "connector.h"

#define BUF_SIZE 100

static void* buf;
static int* shared;

void conn_create()
{
	buf = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);


  	if (buf == MAP_FAILED)
  	{
  		printf("E: map creation\n");
 	}
 	shared = buf;
}

void conn_delete()
{ 
	if (munmap(buf, BUF_SIZE) == -1)
	{
		printf("E: munmap\n");
	}
}

int conn_read(int index)
{

	return shared[index];
}

void conn_write(int index, int number)
{
	shared[index] = number;
}


