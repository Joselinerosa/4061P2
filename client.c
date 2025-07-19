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
int done_reading = 0; // Flag to indicate if reading from file is complete

//for synch
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

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
    q->rear= (q->rear+1) % q->capacity; //update rear index to the NEXT slot in the circular queue, % wraps around when reaching the end of the array 
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
        if (done_reading){
            pthread_mutex_unlock(&queue_mutex); //if no more data is coming and queue is empty, return NULL to signal thread to exit
            return NULL;
        }
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
//TASK 4
void *worker_thread(void *arg){
    int thread_id = *(int*)arg;
    free(arg); // Free the memory allocated for thread ID

    while(1){
        chunk_t *chunk = dequeue(chunk_queue);
        if (!chunk) {
            break;   
        }
    
    // Process the chunk data
    for (int i = 0; i < chunk->length; i++) {
        if (chunk->data[i] >= '0' && chunk->data[i] <= '9') {
            pthread_mutex_lock(&count_mutex); // Lock mutex to safely update digit counts
            digit_counts[chunk->data[i] - '0']++; // Increment the count for the digit
            pthread_mutex_unlock(&count_mutex); // Unlock mutex after updating
        }
    }
        free(chunk); // Free the memory allocated for the chunk after processing
    }
    return NULL; // Exit the thread
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

//TASK 2
    // Allocate memory for an array to hold thread identifiers (pthread_t)
    // Each element in this array corresponds to a single worker thread
    pthread_t *threads = malloc(nthreads *sizeof(pthread_t));
    if (threads ==NULL) {
        // If allocation fails, print error message, clean up queue, and exit
        fprintf(stderr, "Failed to allocate memory for threads\n");
        destroy_queue(chunk_queue);
        return 1; 
    }

    for (int i =0; i< nthreads; i++){
        // Allocate memory for an integer to hold the thread's ID
        // This ID is passed to the thread function as argument
        int *thread_id = malloc(sizeof(int)); 
        if (thread_id ==NULL) {
            // If allocation fails, print error message, clean up queue and previously allocated threads, then exit
            fprintf(stderr, "Failed to allocate memory for thread ID\n");
            destroy_queue(chunk_queue);
            free(threads);
            return 1;
        }
        *thread_id =i; // Assign current loop index as thread ID

        // Create a new thread running the worker_thread function, passing thread_id as argument
        // Store thread identifier in the threads array for later joining
        if (pthread_create(&threads[i], NULL, worker_thread, thread_id) !=0){
            fprintf(stderr, "Failed to create thread %d\n", i);
            destroy_queue(chunk_queue);
            free(threads);
            return 1;
        }
    }

    //Task 3: File partitioning 
    FILE *fp = fopen(filepath, "rb"); // Open the input file in binary read mode ("rb")
    if (fp ==NULL){
        fprintf(stderr, "Failed to open file %s\n", filepath);
        //clean before exiting
        destroy_queue(chunk_queue);
        free(threads);
        return 1;
    }
    
    // Continuously read the file in chunks of size MAX_SIZE (1024 bytes)
    while (1){
        chunk_t chunk;// Create a temporary chunk to hold the read data

        // fread attempts to read MAX_SIZE bytes from file into chunk.data
        // Returns actual number of bytes read (could be less near EOF)
        size_t bytes_read = fread(chunk.data, 1, MAX_SIZE, fp);
        if (bytes_read ==0) { // If zero bytes are read, end of file reached; break the loop
            break; 
        }
        chunk.length = (int)bytes_read; // Store the number of bytes actually read in chunk.length
        enqueue(chunk_queue, &chunk); // Enqueue the filled chunk into the shared queue for worker threads to process enqueue will block if the queue is full, ensuring synchronization
    }
    fclose(fp); //close file when done reading ALL chunks 
    pthread_mutex_lock(&queue_mutex); // Lock the queue mutex to safely update the done_reading flag
    done_reading = 1; // Set the global flag to indicate that reading is complete   
    pthread_cond_broadcast(&not_empty); 
    pthread_mutex_unlock(&queue_mutex); // Unlock the mutex to allow worker threads to proceed


        
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

    for (int i = 0; i <nthreads; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);

    //TASK 7
    printf("Digit counts:\n");
    //loop through each index in the digit_counts array 
    for (int i=0; i<10;i++){
        printf("Digit %d: %d\n", i, digit_counts[i]); //print the total count for each digit collected by all worker threads
    }
            
    destroy_queue(chunk_queue); //free all memory used to protect the queue structure (the enqueue and dequeue) 
    pthread_mutex_destroy(&queue_mutex); //destroy the mutex used to protect the queue structure 
    pthread_mutex_destroy(&count_mutex); //destroy the mutex used to protex updates to the digit_counts[] array 
    pthread_cond_destroy(&not_empty); //destroy the condition variable by producer to WAIT when queue is FULL 
    pthread_cond_destroy(&not_full); //destroy the condition variable used by consuerms to WAIT when queue is EMPTY 
    
    return 0;
}

