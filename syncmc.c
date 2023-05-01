/*
To write a program using c language that writes to a mapped-memory file simultaneously by creating 2 threads and by calling the msync() command with SYNC_MS option, 
synchronize the changes with the external memory,you can use the mmap() command to create a mapped-memory file.
To capture and report TLB related events during program execution on each machine, you can use the perf tool.
To get and report fault pages, number of TLB misses and TLB related events during program execution on a Bare Metal system (without using virtualization) alone, you can use the perf tool.
To run this program on 2 machines at the same time and capture and report TLB related events during program execution on each machine, you can use the perf tool.
To compare the number and amount of TLB shootdown with ASYNC_MS option, you can execute the command with these two options.
Here’s an example code that writes to a mapped-memory file simultaneously by creating 2 threads and by calling the msync() command with SYNC_MS option:
*/

// use perf command:
// https://millyz.github.io/ta/os3150_2016/mem1-lab/mem1/part1_2.html


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define FILEPATH "mmapped.bin"
#define NUM_THREADS 2

void *thread_func(void *arg)
{
    int fd = *(int *)arg;
    char *map;
    map = mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    char *str = "Hello World!";
    memcpy(map, str, strlen(str));
    msync(map, sizeof(char), MS_SYNC);
    munmap(map, sizeof(char));
    close(fd);
}

int main()
{
    int fd;
    pthread_t threads[NUM_THREADS];
    fd = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, sizeof(char) - 1, SEEK_SET) == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }
    if (write(fd, "", 1) == -1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, thread_func, &fd);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
}