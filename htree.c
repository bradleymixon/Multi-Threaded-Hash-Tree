#include <stdio.h>     
#include <stdlib.h>   
#include <stdint.h>  
#include <inttypes.h>  
#include <errno.h>     // for EINTR
#include <fcntl.h>     
#include <unistd.h>    
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>


// Print out the usage of the program and exit.
void Usage(char*);
uint32_t jenkins_one_at_a_time_hash(const uint8_t* , uint64_t );
void *tree(void *arg);
double GetTime();

// block size
#define BSIZE 4096

int 
main(int argc, char** argv) 
{
  int32_t fd;
  uint32_t nblocks;
  int ht;

  // input checking 
  if (argc != 3)
    Usage(argv[0]);

  // open input file
  fd = open(argv[1], O_RDWR);
  if (fd == -1) {
    perror("open failed");
    exit(EXIT_FAILURE);
  }
  // use fstat to get file size
  struct stat sb;
  if(fstat(fd, &sb) == -1){ //filesize in sb.st_size
    perror("fstat error");
    exit(EXIT_FAILURE);
  }
  // calculate nblocks 
  nblocks = sb.st_size / BSIZE; //num of blocks = file size divided by size of each block

  printf(" no. of blocks = %u \n", nblocks);

  double start = GetTime();

  // calculate hash value of the input file
  ht = atoi(argv[2]);
  pthread_t p1;
	pthread_create(&p1, NULL, tree, &ht);
	pthread_join(p1, NULL);


  double end = GetTime();
  //printf("hash value = %u \n", hash);
  printf("time taken = %f \n", (end - start));
  close(fd);
  return EXIT_SUCCESS;
}

uint32_t 
jenkins_one_at_a_time_hash(const uint8_t* key, uint64_t length) 
{
  uint64_t i = 0;
  uint32_t hash = 0;

  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}


void 
Usage(char* s) 
{
  fprintf(stderr, "Usage: %s filename num_threads \n", s);
  exit(EXIT_FAILURE);
}

void* 
tree(void* arg) 
{
	int currHt = *(int*)arg; //find height of current thread
	if(currHt == -1) //Base case of recursion, when height is -1, stop creating new threads
		return NULL;

	if(currHt == 0) //Check if current thread is an internal or leaf node of the binary tree
		printf("Leaf Thread at height %d\n", currHt);
	else
		printf("Int. Thread at height %d\n", currHt);

	pthread_t LChild;
	pthread_t RChild;
	int childHT = currHt - 1; //calculate height of the children of the current thread, height of left and right will be the same

	//Create and join thread for left child of current thread
	pthread_create(&LChild, NULL, tree, &childHT);
	pthread_join(LChild, NULL);

	//Create and join thread for right child of current thread
	pthread_create(&RChild, NULL, tree, &childHT);
	pthread_join(RChild, NULL);

    return NULL;
}

double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double)t.tv_sec + (double)t.tv_usec/1e6;
}
