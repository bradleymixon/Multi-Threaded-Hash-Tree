//Bradley Mixon
//CS3377.505 Assignment 8
//3-29-23

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "common.h"
#include "common_threads.h"

void *tree(void *arg);
int gettid();

int 
main(int argc, char *argv[]) 
{
	int ht;

    if (argc != 2) {
		fprintf(stderr, "usage: htree height \n");
		exit(1);
    }

	ht = atoi(argv[1]);

    pthread_t p1;
	Pthread_create(&p1, NULL, tree, &ht);
	Pthread_join(p1, NULL);
    return 0;
}

// It is easier to write a recursive function than an iterative one.
// Remember arg is an address in the stack of the caller.
// I wouldnt modify the value at that location.
void* 
tree(void* arg) 
{
	int currHt = *(int*)arg; //find height of current thread
	if(currHt == -1) //Base case of recursion, when height is -1, stop creating new threads
		return NULL;

	if(currHt == 0) //Check if current thread is an internal or leaf node of the binary tree
		printf("Leaf Thread (id: %d) at height %d\n", gettid(), currHt);
	else
		printf("Int. Thread (id: %d) at height %d\n", gettid(), currHt);

	pthread_t LChild;
	pthread_t RChild;
	int childHT = currHt - 1; //calculate height of the children of the current thread, height of left and right will be the same

	//Create and join thread for left child of current thread
	Pthread_create(&LChild, NULL, tree, &childHT);
	Pthread_join(LChild, NULL);

	//Create and join thread for right child of current thread
	Pthread_create(&RChild, NULL, tree, &childHT);
	Pthread_join(RChild, NULL);

    return NULL;
}

int
gettid()
{
	return (syscall(SYS_gettid));
}
