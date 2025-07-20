# Programming Assignment 1: Integer Sum and Average Calculation

## Team Information
1. Team members: Joseline Rosa & Peyton Tregembo
2. x500: Rosa0168 & Trege006
3. Division of tasks:
Task 1: Joseline and Peyton
Task 2: Peyton
Task 3: Peyton
Task 4: Joseline
Task 5: Joseline
Task 6: Joseline
Task 7: Peyton 
README.md: Peyton 
5. Lab machine used: SSH trege006@csel-khl1260-17.cselabs.umn.edu 
                     SSH rosa0168@csel-khl1260-17.cselabs.umn.edu 

## Design Specifications

### Phase 1 Design
- implemented a multi-thread producer-consumer model using pthreads. The producer thread reads an input file in fixed size chunks (1024 bytes) and enqueues these chunks into a shared queue.
- Multiple worker threads dequeue chunks from the enqueu and process them concurrently to count digit occurrences (0 through 9) within the data. Uses synchronization- a mutex (queue_mutex)
- protects the queue data structure during enqueue and dequeue operations. Conditional variables (not_empty and not_full) coordinate producer and consumer threads to wait and signal when the queue
- is empty or full. Count_mutex ensures safe updates to the shared digit counts array. The producer signals completion by setting a done_reading flag and telling all waiting consumer threads.
- Consumer threads terminate once the queue is empty and no more data will be produced. The main thread waits for all worker threads to finish, then prnits the total digit counts and cleans up
- all the resources (mutexs, conditon varibales, allocated memory) 

### Phase 2 Design

## Challenges Faced
Peyton:
- Debugging in VSCode
- designing thread-safe enqueue and dequeue functions with proper synchornization using mutexes and condition variables was pretty complex.
- Ensuring no deadlocks or race conditions occured while managing the circular queue 

## AI Tools Usage
Peyton: I used AI to better understand pthread synchonization concepts in order to improve the queue implementation. 
Joseline: I used AI to help me understand the concept of worker_thread to then implement it. 


## Additional Notes
[TODO: Any other information for the TA]
