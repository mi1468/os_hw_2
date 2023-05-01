/*
C program that calls the madvise() command a large number of times with a variable number of strings by following these steps:

1- Receive the number of threads as input from the user (the maximum number of selected threads should be one less than the number of processor cores).
2- Each thread should be executed only on a specific core.
3- Synchronization or synchronization of threads should be done by barrier_pthread.
4- The exact execution time of the program should be recorded according to the number of different strings.

Here’s an example code that you can use as a starting point:
*/
// use perf command:
// https://millyz.github.io/ta/os3150_2016/mem1-lab/mem1/part1_2.html

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>

#define MAX_THREADS 4
#define MAX_STRINGS 10
#define MAX_STRING_LENGTH 100

pthread_barrier_t barrier;
int num_threads;
char **strings;
int num_strings;
int *core_ids;

void *thread_func(void *arg) {
    int thread_id = *(int *) arg;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_ids[thread_id], &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    pthread_barrier_wait(&barrier);
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    for (int i = 0; i < num_strings; i++) {
        madvise(strings[i], strlen(strings[i]), MADV_NORMAL );
    }
    gettimeofday(&end_time, NULL);
    long long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000LL + end_time.tv_usec - start_time.tv_usec;
    printf("Thread %d take %lld micro_S \n", thread_id, elapsed_time);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s num_threads\n", argv[0]);
        exit(1);
    }
    num_threads = atoi(argv[1]);
    if (num_threads > MAX_THREADS) {
        printf("Error: Maximum number of threads exceeded\n");
        exit(1);
    }
    strings = malloc(MAX_STRINGS * sizeof(char *));
    for (int i = 0; i < MAX_STRINGS; i++) {
        strings[i] = malloc(MAX_STRING_LENGTH * sizeof(char));
        sprintf(strings[i], "String %d", i);
    }
    num_strings = MAX_STRINGS;
    core_ids = malloc(num_threads * sizeof(int));
    for (int i = 0; i < num_threads; i++) {
        core_ids[i] = i + 1;
    }
    pthread_barrier_init(&barrier, NULL, num_threads);
    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}
