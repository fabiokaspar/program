#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
#define M 5 

pthread_barrier_t barrier;

void *f_thread(void* v) {
  long id = (long) v;
  int i;

  for (i = 1; i < 10; i++) {
    sleep(random() % 3 + 1); 
    printf("Thread %ld esta na %d rodada.\n", id, i);
    pthread_barrier_wait(&barrier);
    // printf("Thread %ld passou pela barreira.\n", id);
  }
  return NULL;
}

int main() {
  pthread_t thr[N];
  long i;

  // srandom(time(NULL));
  pthread_barrier_init(&barrier, NULL, M);
  for (i = 0; i < N; i++) 
    pthread_create(&thr[i], NULL, f_thread, (void*) i);

  pthread_exit(NULL);
}