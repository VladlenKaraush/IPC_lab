#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "connector.h"

#define CLIENTS_NUMBER 4


static char* namesFIFO[CLIENTS_NUMBER] = {"fifo1","fifo2","fifo3","fifo4"};
static int fd[CLIENTS_NUMBER];

void conn_create()
{
	int i;
	for(i = 0; i < CLIENTS_NUMBER;i++)
	{
		if (mkfifo(namesFIFO[i], S_IRWXU ) == -1)
		{
			printf("E: With creating of fifo\n");
		}
		fd[i] = open(namesFIFO[i], O_RDWR | O_NONBLOCK);
	}

}

void conn_delete()
{
	int i;
	for(i = 0; i < CLIENTS_NUMBER; i++)
	{
		close(fd[i]);
		unlink(namesFIFO[i]);
	}

}

int conn_read(int index)
{

	int  size, value;
	size = read(fd[index], &value, sizeof(int));
	if (size == -1)
	{
		printf("E: With reading in fifo\n");
	}

	return value;

}

void conn_write(int index, int number)
{

	write(fd[index], &number, sizeof(int));
}


