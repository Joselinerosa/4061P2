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

//for synch
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

//struct def
typedef struct chunk {
    char data[MAX_SIZE]; // Buffer to hold chunk data
    int length; // Length of the data in the chunk
} chunk_t;

typedef struct queue {
    int front;
    int rear;
    int size;
    int capacity;
    chunk_t *chunks; // ARRAY of chunks
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
    //need to prevent race conditions when multiple threads try to enqueue/dequeue concorrently 
    pthread_mutex_lock(&queue_mutex); //lock mutex for exclusive acces to the queue data structure 

    //if the queue is full, wait on the not_full condition variable
    //this means the producer thread will SLEEP here until a worker thread signals that a slot has been freed (by dequeuing a chunk) 
    while (q->size == q->capacity) {
        //this atomically releases the mutex and suspends the thread
        //when the thread wakes up, it re-aquires the mutex before returning 
        pthread_cond_wait(&not_full, &queue_mutex); //this waits if the queue is FULL 
    //return 0;
    }

    
    q->chunks[q->rear] = *chunk; //copy chunk data into the position indicated by the rear index in the circular buffer
    q->rear= [q->rear+1] % q->capacity; //update rear index to the NEXT slot in the circular queue, % wraps around when reaching the end of the array 
    q->size++; //must increment size to reflect the ADDED chunk 

    pthread_cond_signal(&not_empty); //signal the not_empty condition variable to wake up any threads WAITING to dequeu, queue not has at least one item
    pthread_mutex_unlock(&queue_mutex); // unlock the mutex to allow other threads to access the queue 
    return 0; //success 
}





chunk_t* dequeue(queue_t *q) {
    // TODO: Remove and return chunk from queue
    // Lock the mutex for exclusive access to the queue 
    pthread_mutex_lock(&queue_mutex);

    //if queue is empty, wait on the not_empty condition variable 
    //this means the consumer thread will sleep here until a producer signals that a chunk has been ENQUEUED 
    while (q->size==0) {
        pthread_cond_wait(&not_empty, &queue_mutex); //waits if queue is EMPTY, releases mutex and suspends thread until signaled 
    }
    //must allocate memory for a chunk pointer that will hold the dequeued data 
    chunk_t *chunk = malloc(sizeof(chunk_t));
    if(chunk ==NULL){
        //if memory allocation fails- print error message AND unlock mutex, returns null 
        fprintf(stderr, "Failed to allocate memory for dequeued chunk\n");
        pthread_mutex_unlock(&queue_mutex);
        return NULL;
    }
    //copy that chunk data from the position indicated by front in the queue
    *chunk = q->chunks[q->front];
    q->front = (q->front +1) % q->capacity; //update front index to NEXT slot, 
    q->size--; //must decrement the queue size to reflect REMOVAL 

    pthread_cond_signal(&not_full); //signal the not_full condition vairable to wake up any threads waiting to enqueue
    pthread_mutex_unlock(&queue_mutex); // unlock the mutex so other threads can access the queue 
    return chunk; //return the POINTER to the dequeued chunk 
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

