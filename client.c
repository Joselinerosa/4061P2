#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "client.h"


// Global variables
queue_t *chunk_queue;
int digit_counts[10] = {0};
#define MAX_SIZE 1024
typedef struct chunk {
    char data[MAX_SIZE]; // Buffer to hold chunk data
    int length;          // Length of the data in the chunk
} chunk_t;

typedef struct queue {
    int front;
    int rear;
    int size;
    int capacity;
    chunk_t *chunks; // Array of chunks
} queue_t;





// Queue implementation functions
queue_t* create_queue(int qsize) {
    // TODO: Implement queue creation
    
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL) {
        fprintf(stderr, "Failed to allocate memory for queue\n");
        return NULL;
    }
    q->capacity = qsize;
    q->front = 0;
    q->rear = 0;
    q->size = 0;
    q->chunks = malloc(qsize * sizeof(chunk_t));
    if (q->chunks == NULL) {
        fprintf(stderr, "Failed to allocate memory for queue chunks\n");
        free(q);
        return NULL;    
}
    return q;
}

void destroy_queue(queue_t *q) {
    // TODO: Free queue memory
    if (q != NULL) {
        free(q->chunks);
        free(q);
    }   
    else{
        fprintf(stderr, "Queue is NULL, cannot destroy\n");
    }
}

int enqueue(queue_t *q, chunk_t *chunk) {
    // TODO: Add chunk to queue
    return 0;
}

chunk_t* dequeue(queue_t *q) {
    // TODO: Remove and return chunk from queue
    return NULL;
}

// Worker thread function
void* worker_thread(void *arg) {
    int thread_id = *(int*)arg;
    
    // TODO: Main worker loop
    while (1) {
    }
    
    return NULL;
}

// Main function
int main(int argc, char *argv[]) {
    // Parse command line arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s [filepath] [qsize] [nthreads] [client_id]\n", argv[0]);
        return 1;
    }
    
    char *filepath = argv[1];
    int qsize = atoi(argv[2]);
    int nthreads = atoi(argv[3]);
    int client_id = atoi(argv[4]);

    
    // TODO: Initialize synchronization primitives
        
    // TODO: Create queue
    chunk_queue = create_queue(qsize);
    if (chunk_queue == NULL) {
        fprintf(stderr, "Failed to create queue\n");
        return 1;
    }
    else{
        printf("Queue created with capacity %d\n", qsize);
    }



        
    // TODO: Create worker threads
        
    // TODO: Open and read file in chunks
    // TODO: Read file in 1024-byte chunks
    // TODO: Operates as the producer
    
    // TODO: When all bytes have been read from the file and placed into the
    // queue, Signal completion and wait for workers
        
    // TODO: Print results
        
    // TODO: Cleanup
    // Destroy queue
    // Destroy mutexes and condition variables
    // Free allocated memory
    destroy_queue(chunk_queue);
    return 0;
}

