#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define ITERATIONS 20
#define BUFFER_SIZE 3

int A = 0, B = 0;
int buffer[BUFFER_SIZE];
sem_t mutexA, mutexB;
sem_t s, n, e; // s -> read and write  n -> consumer  e -> producer

void* producer(void* arg) {
    int tid = *((int*)arg);
    printf("Thread %d - Producer\n", tid); // Print thread ID
    for (int i = 0; i < ITERATIONS; ++i) {
        sem_wait(&mutexA); // Lock mutexA to protect A
        A += 1; // Critical section for A
        sem_post(&mutexA); // Release mutexA
        usleep(rand() % 101); // Introduce random sleep

        sem_wait(&mutexB); // Lock mutexB to protect B
        B += 3; // Critical section for B
        sem_post(&mutexB); // Release mutexB
        usleep(rand() % 101); // Introduce random sleep

        int item = rand() % 100;
        printf("Producer %d produced %d\n", tid, item);
        semWait(e);
        semWait(s);

    }
    pthread_exit(NULL);
}

void* consumer(void* arg) {
    int tid = *((int*)arg);
    printf("Thread %d - Consumer\n", tid); // Print thread ID
    for (int i = 0; i < ITERATIONS; ++i) {
        sem_wait(&mutexB); // Lock mutexB to protect B
        B += 3; // Critical section for B
        sem_post(&mutexB); // Release mutexB
        usleep(rand() % 101); // Introduce random sleep

        sem_wait(&mutexA); // Lock mutexA to protect A
        A += 1; // Critical section for A
        sem_post(&mutexA); // Release mutexA
        usleep(rand() % 101); // Introduce random sleep
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <NUM_THREADS>\n", argv[0]);
        return 1;
    }

    int NUM_THREADS = atoi(argv[1]);
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    sem_init(&mutexA, 0, 1);
    sem_init(&mutexB, 0, 1);
    sem_init(&s, 0, 1); // s is initialized to 1
    sem_init(&n, 0, 0); // n is initialized to 0
    sem_init(&e, 0, BUFFER_SIZE); // e is initialized to sizeofbuffer

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (i % 2 == 0) {
            pthread_create(&threads[i], NULL, producer, &thread_ids[i]);
        } else {
            pthread_create(&threads[i], NULL, consumer, &thread_ids[i]);
        }
    }

    printf("Launching %d threads\n", NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    

    printf("Final value of A: %d\n", A);
    printf("Final value of B: %d\n", B);

    sem_destroy(&mutexA);
    sem_destroy(&mutexB);

    return 0;
}