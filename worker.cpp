#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <cstdio>
#include <cstdlib>

#define PERMS 0644
typedef struct msgbuffer
{
	long mtype;
	char strData[100];
	int intData;
} msgbuffer;

int *shm_ptr;

// Function to attach to shared memory
void shareMem()
{
	// Generate key
	const int sh_key = ftok("main.c", 0);
	// Access shared memory
	int shm_id = shmget(sh_key, sizeof(int) * 2, 0666);

	// Determine if shared memory access not successful
	if (shm_id == -1)
	{
		// If true, print error message and exit 
		fprintf(stderr, "Child: Shared memory get failed.\n");
		exit(1);
	}

	// Attach shared memory
	shm_ptr = (int *)shmat(shm_id, 0, 0);
	//Determine if insuccessful
	if (shm_ptr == (int *)-1)
	{
		// If true, print error message and exit
		fprintf(stderr, "Child: Shared memory attach failed.\n");
		exit(1);
	}
}


int main(int argc, char* argv[])
{
	msgbuffer buf;
	buf.mtype = 1;
	int msqid = 0;
	key_t key;

	// Get key for message queue
	if ((key = ftok("msgq.txt", 1)) == -1)
	{
		perror("ftok");
		exit(1);
	}

	// Create message queue
	if ((msqid = msgget(key, PERMS)) == -1)
	{
		perror("msgget in child\n");
		exit(1);
	}

	printf("Child %d has access to the queue.\n", getpid());

	// Receive a message, but only one for us
	if (msgrcv(msqid, &buf, sizeof(msgbuffer), getpid(), 0) == -1)
	{
		perror("Failed to receive message from parent.\n");
		exit(1);
	}

	// Output message from parent
	printf("Child %d received message: %s was my message and my int data was %d\n", getpid(), buf.strData, buf.intData);

	// Send message back to parent
	buf.mtype = getppid();
	buf.intData = getppid();
	strcpy(buf.strData, "Message back to muh parent\n");

	if (msgsnd(msqid, &buf, sizeof(msgbuffer)-sizeof(long), 0) == -1)
	{
		perror("msgsnd to parent failed.\n");
		exit(1);
	}

	printf("Child %d is now ending\n", getpid());
	return 0;
}
