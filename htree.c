#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <inttypes.h>

#include <errno.h> // for EINTR

#include <fcntl.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h> //for GetTime()
#include <assert.h> //for GetTime()

// Print out the usage of the program and exit.

void Usage(char*);

uint32_t jenkins_one_at_a_time_hash(const uint8_t* , uint64_t );
void *thread_function(void *args);
double GetTime();

typedef struct {
    int32_t fd;
    uint32_t start_block;
    uint32_t blocks_per_thread;
    uint32_t thread_number;
} ThreadArgs;

// block size

#define BSIZE 4096

int

main(int argc, char** argv)

{

int32_t fd;

uint32_t nblocks;

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
  if(fstat(fd, &sb) == -1){
    perror("fstat error");
    exit(EXIT_FAILURE);
  }
  //filesize in sb.st_size

// calculate nblocks
nblocks = sb.st_size / BSIZE;

printf(" no. of blocks = %u \n", nblocks);

double start = GetTime();

// calculate hash value of the input file
uint32_t num_threads = atoi(argv[2]);
pthread_t threads[num_threads];
ThreadArgs thread_args[num_threads];

for (uint32_t i = 0; i < num_threads; i++) {
    thread_args[i].fd = fd;
    thread_args[i].start_block = i * (nblocks / num_threads);
    thread_args[i].blocks_per_thread = nblocks / num_threads;
    thread_args[i].thread_number = i;
    pthread_create(&threads[i], NULL, thread_function, (void *)&thread_args[i]);
}

uint32_t final_hash = 0;
uint32_t thread_hash;
for (uint32_t i = 0; i < num_threads; i++) {
    pthread_join(threads[i], (void **)&thread_hash);
    final_hash ^= thread_hash;
}
printf("hash value = %u \n", final_hash);

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

double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double)t.tv_sec + (double)t.tv_usec/1e6;
}


void *thread_function(void *args) {
    ThreadArgs *ta = (ThreadArgs *)args;
    uint8_t buffer[BSIZE];
    uint32_t block_hash;
    uint64_t hash = 0;
    for (uint32_t i = 0; i < ta->blocks_per_thread; i++) {
        pread(ta->fd, buffer, BSIZE, (ta->start_block + i) * BSIZE);
        block_hash = jenkins_one_at_a_time_hash(buffer, BSIZE);
        hash ^= block_hash;
    }
    pthread_exit((void *)(uintptr_t)hash);
}
