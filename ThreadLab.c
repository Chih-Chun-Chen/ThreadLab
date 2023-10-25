#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS 4 // Change this to the desired number of threads

void *producer(void *threadid) {
    long tid;
    tid = (long)threadid;
    printf("I am a producer, TID: %ld\n", tid);
    pthread_exit(NULL);
}

void *consumer(void *threadid) {
    long tid;
    tid = (long)threadid;
    printf("I am a consumer, TID: %ld\n", tid);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;

    for (t = 0; t < NUM_THREADS; t++) {
        if (t % 2 == 0) {
            rc = pthread_create(&threads[t], NULL, producer, (void *)t);
        } else {
            rc = pthread_create(&threads[t], NULL, consumer, (void *)t);
        }
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            return -1;
        }
    }

    pthread_exit(NULL);
}
