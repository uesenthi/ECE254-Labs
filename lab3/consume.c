#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <signal.h>
#include "common.h"
#include "point.h"
#include <math.h>
#include <sys/mman.h>

#define _XOPEN_SOURCE 600

int main(int argc, char *argv[])
{

	mqd_t qdes;
	char  qname[] = "/mailbox1_uesenthi";
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	int childstatus;
	int number=0;
	int consume_count=0;
	
	//Check for the right number of arguments
	if ( argc !=6 ) {
		printf("Usage: %s <qname>\n", argv[0]);
		printf("The number of arguments is actually %d\n", argc);
		exit(1);
	}
	
	//Get input arguments
	int N = atoi(argv[1]); 
	int B = atoi(argv[2]);
	int P = atoi(argv[3]);
	int C = atoi(argv[4]);
	int receiverID = atoi(argv[5]);

	attr.mq_maxmsg  = B;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags   = 0;	//queue can be blocked  

	qdes  = mq_open(qname, O_RDONLY, mode, &attr);
	if (qdes == -1 ) {
		perror("mq_open()");
		exit(1);
	}
	
	while (consume_count < N) {
		if ((mq_receive(qdes, (char *) &number, sizeof(int), 0)) == -1) {
		perror("mq_receive() failed");	
		} 
		//Consume count is only used for C = 1
		//If more than one consumer, faster processes stop prematurely to avoid locking the others
		consume_count++;
		if (C > 1 && number >= N-C) {
			break;
		}
		
		if (number == floor(sqrt(number))*floor(sqrt(number))){
			printf("%d %d %d \n", (int)receiverID, (int)number, (int)sqrt(number));
			
		}

	}

	//Once done, close the queue
	if (mq_close(qdes) == -1) {
		perror("mq_close() failed");
		exit(2);
	}
	
	wait(&childstatus);

	return 0;
}
