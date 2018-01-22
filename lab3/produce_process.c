
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include "common.h"
#include "point.h"
#include <sys/wait.h>
#include <sys/types.h>

int spawn(char* program, char** arg_list)
{
		pid_t child_pid;
		child_pid = fork();
		if (child_pid != 0)
			//Parent Process: return the child process id
			return child_pid;
		else {
			//If child process is for producer, do not open a new program and return
			if (program != "./consume"){
				return 0;
			}
			else 
			{
				//open new consumer program
				execvp(program, arg_list);
				//only returns if there is an error
				fprintf(stderr, "an error occurred in execvp\n");
				abort();
			}
		}
}

int main(int argc, char *argv[])
{
	struct timeval duration;
	double timestart;
	double timeend;
	int child_status;

	int senderID = 0;	//Parent process is the first sender, so ID=1
	int* receiverID;	
	
	//List of arguments for the receiver process
	char* arg_list[] = {"./consume", argv[1], argv[2], argv[3], argv[4], (char*)receiverID, NULL};
	
	//Check valid number of input arguments
	if ( argc !=5 ) {
		printf("Usage: %s <qname>\n", argv[0]);
		printf("The qname must start with a \"/\". \n");
		printf("An example qname: /mailbox1_userid\n");
		exit(1);
	}
	//Get the input parameters
	int N = atoi(argv[1]); 
	int B = atoi(argv[2]);
	int P = atoi(argv[3]);
	int C = atoi(argv[4]);
	
	//Open message 	queue
	mqd_t qdes;
	char  qname[] = "/mailbox1_uesenthi";	
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;
	attr.mq_maxmsg  = B;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags   = 0;	
	//Delete queue if it's still available
	//mq_unlink(qname);

	qdes  = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
	if (qdes == -1 ) {
		perror("mq_open() failed");
		exit(1);
	}

	srand(time(0));
	
	//Spawn receiver process
	int i;
	for (i = 0; i < C; i++)
	{
		spawn("./consume", arg_list); //If parent returns, increase receiverID
	}
	//spawn producer processes
	for (i = 0; i < P-1; i++)
	{
		if (spawn("./produce_process", arg_list) != 0){ //If parent returned, increase senderID and continue loop
			senderID++;
		}
		else {break;}	//If child returned, break out of loop
	}
	
	
	gettimeofday(&duration, NULL);
	timestart = duration.tv_sec +  duration.tv_usec/1000000.0;
	
	do {
			if (mq_send(qdes, (char *)&senderID, sizeof(int), 0) == -1) {
				perror("mq_send() failed");
			}
			senderID = senderID + P;	//After send, increment senderID to next value

	} while (senderID < N); //quit after senderID exceeds N

	//Once done producing, all child processes exit and parents wait
	if (mq_close(qdes) == -1) {
		perror("mq_close() failed");
		exit(2);
	}
	wait(&child_status);
	if (senderID%N != 0){	//Parent process skips this and waits for all child processes
		return 0;
	}

	//Get total system execution time
	gettimeofday(&duration, NULL);
	timeend = duration.tv_sec +  duration.tv_usec/1000000.0;
	printf("System execution time: %f seconds\n",(timeend-timestart));


	return 0;

}
