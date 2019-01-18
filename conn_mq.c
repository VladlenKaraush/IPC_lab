#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <mqueue.h>
#include "connector.h"

#define CLIENTS_NUMBER 4

static char* namesMQ[CLIENTS_NUMBER] = {"/mq1","/mq2","/mq3","/mq4"};


static mqd_t queue[CLIENTS_NUMBER];
static struct mq_attr attr;

void conn_create()
{
	int i;
	attr.mq_maxmsg = 1;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags = 0;
	attr.mq_curmsgs = 0;

	for(i = 0; i < CLIENTS_NUMBER;i++)
	{
        if ((queue[i] = mq_open(namesMQ[i], O_CREAT | O_RDWR,  S_IRWXU, &attr)) == -1)
            printf("E: With opening mq\n");

	}
}

void conn_delete()
{
	int i;
	for(i = 0; i < CLIENTS_NUMBER; i++)
	{
		mq_close(queue[i]);
		mq_unlink(namesMQ[i]);
	}

}

int conn_read(int index)
{
	int value, size;
	unsigned int pr;
	if ((size = mq_receive(queue[index], (char*)&value, sizeof(int), &pr)) == -1)
	{
		printf("E: With reading in mq\n");
	}
	return value;

}

void conn_write(int index, int number)
{
	if (mq_send(queue[index], (char*)&number, sizeof(int), 0) == -1)
		printf("E: With writing in mq\n");
}

