# Multi-Threaded-Hash-Tree
CS/SE 3377 Sys. Prog. in Unix and Other Env. Project 2: Multi-threaded Hash Tree

In this project, you will write a multi-threaded program to compute a hash value of a given file. The size of the file can be very large. So, there is a need for the program to be multi-threaded to achieve speedup. While one thread reads a part (block) of the file, other threads can compute hash values of already read blocks.

## Hash Value
This project uses [Jenkins one_at_a_time hash](https://en.wikipedia.org/wiki/Jenkins_hash_function).
We will restrict the hash value type to a 32-bit unsigned integer.

## Testing
The program compiles as follows:
gcc htree.c –o htree -Wall -Werror -std=gnu99 -pthread

# Part 1: Multi-threaded hashing
A file can be divided into multiple blocks. The basic unit of accessing a file on a i/o device is a block. Assume there are n blocks, m threads, and n is divisible by m. Each thread can compute the hash value of n/m consecutive blocks. Several of these hash values are then combined to form a string whose hash value can be computed as before. These steps can be repeated till one final hash value remain.

As in assignment 8, a complete binary tree of threads should be created. Each leaf thread will compute the hash value of n/m consecutive blocks assigned to it and return the hash value to its parent thread (through pthread_exit() call). An interior thread will compute the hash value of the blocks assigned to it, and then wait for its children to return hash values. These hash values should be combined to form a string: <computed hash value + hash value from left child + hash value from right child>. (Note that the + sign here means concatenation and not addition. Also, if the interior thread has no right child, only the hash value of the interior thread and the hash value from the left child are combined.) The interior thread then computes hash value of the concatenated string as before and returns the value to its parent, The value returned by the root thread is the hash value of the file.

## How to assign blocks to threads?
Each thread is assigned a number when it is created. The number of root thread is 0. For a thread with number i, the number of its left child is 2 * i + 1, and the number of its right child is 2 * i + 2. For a thread with number i, n/m consecutive blocks starting from block number i * n/m are assigned. For example, let n = 100 and m = 4. Then thread 0 will be assigned blocks 0 through 24, thread 1 will be assigned blocks 25 through 49, thread 2 will be assigned blocks 50 through 74, and thread 3 will be assigned blocks 75 through 99.

# Part 2: Experimental study
The main goal of this study is to find speedup achieved when the number of threads is increased for various input files. For each input file, find the time taken to compute the hash value for 1, 4, 16, 32, 64, 128, and 256 threads.

Plot a graph with time on y-axis, and #threads on x-axis for each input file. Plot another graph with speedup on y-axis and #threads on x-axis for each input file.
speedup = (time taken for single thread)/(time taken for n threads)

Project Report:

The experimental setup consists of a Linux server (cs3.utdallas.edu) with 48 CPUs. Based on observations from the included graphs, it can be generally expected that the time taken to compute the hash value decreases when the number of threads increases. There are exceptions when this does not happen, which can likely be attributed to a normal deviation in CPU performance, possibly due to the number of users connected to the server or other outside factors. Yet in almost every case, the run time decreases as the number of threads increased. However, the speedup achieved increased as the number of threads also increased. The speedup is not directly proportional to the number of threads. The biggest difference is seen in the run time for 1 thread vs the run time for 4 threads. Going from 1 to 4 makes a large difference, but every incremental addition to the number of threads after that only results in a marginal decrease in runtime.



