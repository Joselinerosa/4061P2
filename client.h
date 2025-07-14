#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>

#define CHUNK_SIZE 1024

// Structure for a chunk of data
// Add any code you think necessary to implement your design
typedef struct chunk {
} chunk_t;

// Structure for the queue
// Add any code you think necessary to implement your design
typedef struct queue {
} queue_t;

// Function prototypes
queue_t* create_queue(int capacity);
void destroy_queue(queue_t *q);
int enqueue(queue_t *q, chunk_t *chunk);
chunk_t* dequeue(queue_t *q);
void* worker_thread(void *arg);
#endif // CLIENT_H
