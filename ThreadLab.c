#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define BUF_SIZE 3

int A = 0;
int B = 0;
int buffer[BUF_SIZE];
int fill = 0;
int use = 0;
int count = 0;

sem_t empty;
sem_t full;
sem_t mutexA;
sem_t mutexB;
sem_t s;

void *producer(void *threadid) {
    long tid;
    tid = (long)threadid;
    int i, item;
    for (i = 0; i < 100; i++) {
        usleep(rand() % 101);
        sem_wait(&mutexA);
        A += 1;
        sem_post(&mutexA);
        sem_wait(&mutexB);
        B += 3;
        sem_post(&mutexB);
    }

    for (int j = 0; j < 20; j++) {
        item = rand() % 100;
        printf("%ld: Produces %d\n", tid, item);
        sem_wait(&empty);
        sem_wait(&s);
        buffer[fill] = item;
        fill = (fill + 1) % BUF_SIZE;
        sem_post(&s);
        sem_post(&full);
    }
    pthread_exit(NULL);
}

void *consumer(void *threadid) {
    long tid;
    tid = (long)threadid;
    int i, item;
    for (i = 0; i < 100; i++) {
        usleep(rand() % 101);
        sem_wait(&mutexB);
        B += 3;
        sem_post(&mutexB);
        sem_wait(&mutexA);
        A += 1;
        sem_post(&mutexA);
    }

    for (int j = 0; j < 20; j++) {
        sem_wait(&full);
        sem_wait(&s);
        item = buffer[use];
        use = (use + 1) % BUF_SIZE;
        sem_post(&s);
        sem_post(&empty);
        printf("%ld: Consumes %d\n", tid, item);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return -1;
    }

    int NUM_THREADS = atoi(argv[1]);
    pthread_t threads[NUM_THREADS];
    long t;
    srand(time(NULL));

    sem_init(&empty, 0, BUF_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutexA, 0, 1);
    sem_init(&mutexB, 0, 1);
    sem_init(&s, 0, 1);

    for (t = 0; t < NUM_THREADS; t++) {
        if (t % 2 == 0) {
            pthread_create(&threads[t], NULL, producer, (void *)t);
        } else {
            pthread_create(&threads[t], NULL, consumer, (void *)t);
        }
    }

    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    printf("Final value of A: %d\n", A);
    printf("Final value of B: %d\n", B);
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutexA);
    sem_destroy(&mutexB);
    pthread_exit(NULL);
    return 0;
}