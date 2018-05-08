/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------

*/

//Please enter your name and McGill ID below
//Name: Sheikh Saad Salah Uddin
//McGill ID: 260569832

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>
#include <time.h>

#define RUN_TIME 10	//used when running for 10 "hours"

int BUFFER_SIZE = 100; //size of queue
sem_t full; //semaphore keeping queue count
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; //mutex variable for critical section protection

// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};

struct Queue* queue;
 
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
 
// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{
    return ((queue->size ) >= queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
 
// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("Passenger %d enqueued to queue.\n", item);
}

// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void print(struct Queue* queue){
    if (queue->size == 0){
        return;
    }
    
    for (int i = queue->front; i < queue->front +queue->size; i++){
        
        printf(" Element at position %d is %d \n ", i % (queue->capacity ), queue->array[i % (queue->capacity)]);
    }
    
}

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void* cl_id)
{
  //int j = 0; //used for storing the hour count when running for 10 "hours"
  //while(j < RUN_TIME){ //this while loop condition used when running for 10 "hours"
  while(1){ //infinite loop of plane arrivals
    int planeID = *(int *)cl_id;

    int people = (rand()%(6)+5); // randomly generate between 5 to 10 people
    printf("Airplane %d arrives with %d passengers.\n", planeID, people);
    //enqueue
    for (int i = 0; i < people; i++){
      if(isFull(queue)){
        printf("Rest of passengers of plane %d took the bus.\n", planeID);
        i = people; // so that the for loop is exited here
      }else{
        pthread_mutex_lock(&m); // beginning of critical section
        enqueue(queue, (i+(1000*planeID)+1000000)); //
        pthread_mutex_unlock(&m); // end of critical section 
        sem_post(&full); // increment full
      }
    }
    sleep(1); // sleeping for 1 "hour"
    //j++; //increment the hour count when running for 10 "hours"
  }
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport*/ 
void *FnTaxi(void* pr_id)
{
  int taxiID = *(int *)pr_id; //variable to store the taxi ID for use
  int waitingTaxi = 0; //variable used to not repeatedly print the same waiting taxi

    while(1){ //infinite loop of taxi arrivals
      int taxitime = (rand()%(340000)+170000);  //generate random number between 0.17 and 0.5 seconds
      if(isEmpty(queue)){
        if(waitingTaxi == 0){
          printf("Taxi driver %d arrives.\n", taxiID);
          printf("Taxi driver %d waiting for passenger to arrive in platform.\n", taxiID);
          waitingTaxi = 1; // so that the current waiting taxi does not repeatedly print
        }
      }else{
        printf("Taxi driver %d arrives.\n", taxiID);
        sem_wait(&full); //decrement full, i.e waiting for passenger to be in queue
        pthread_mutex_lock(&m); //beginning of critical section
        printf("Taxi driver %d picked up client %d.\n", taxiID, queue->array[queue->front]);
        dequeue(queue); //remove a passenger from queue
        pthread_mutex_unlock(&m); //end of critical section
        waitingTaxi = 0; //to allow the taxi to wait when the queue again becomes empty
        usleep(taxitime);//sleep randomly between 10 and 30 "minutes"
      }
    }
}

int main(int argc, char *argv[])
{

  int num_airplanes;
  int num_taxis;

  num_airplanes=atoi(argv[1]);
  num_taxis=atoi(argv[2]);

  //exit if incorrect number of arguments
  if(argc != 3){
    printf("Incorrect number of arguments entered.\n");
    exit(EXIT_FAILURE);
  }

  //because number of airplanes and taxis must be positive and less than 1000
  if(num_airplanes < 1 || num_airplanes > 999 || num_taxis < 1 || num_taxis > 999){
    printf("Incorrect value entered for at least one of the arguments.\n");
    exit(EXIT_FAILURE);
  }

  printf("You entered: %d airplanes per hour\n",num_airplanes);
  printf("You entered: %d taxis\n", num_taxis);
  
  //srand(time(NULL)); //seed random number generator to prevent generating same number
  time_t now;
  srand((unsigned int)(time(&now)));
  
  //initialize queue
  queue = createQueue(BUFFER_SIZE);
  
  //declare arrays of threads and initialize semaphore(s)
  pthread_t planeIDs[num_airplanes];
  pthread_t taxiIDs[num_taxis];

  //initialize semaphore which keeps count of how full the buffer is
  if(sem_init(&full, 0, 0)){
    printf("Semaphore initialisation failed.\n"); //exit if semaphore initialisation failed
    exit(EXIT_FAILURE);
  }

  //create arrays of integer pointers to ids for taxi / airplane threads
  int *taxi_ids[num_taxis];
  int *airplane_ids[num_airplanes];
  
  //create threads for airplanes
  for (int i = 0; i < num_airplanes; i++){
    airplane_ids[i] = malloc(sizeof(int));
    *airplane_ids[i] = i;
    if(pthread_create(&planeIDs[i], NULL, FnAirplane, airplane_ids[i]) == 0){
        printf("Creating plane thread %d.\n", i);
	}
    else{
	printf("Plane thread %d creation failed.\n", i);
	}
  }  
  
  //create threads for taxis
  for (int i = 0; i < num_taxis; i++){
    taxi_ids[i] = malloc(sizeof(int));
    *taxi_ids[i] = i;
    if(pthread_create(&taxiIDs[i], NULL, FnTaxi, taxi_ids[i]) != 0){
	printf("Taxi thread %d creation failed.\n", i);
	}
  } 

  //Join plane thread after its work has finished
  for (int i = 0; i < num_airplanes; i++){
    if(pthread_join(planeIDs[i], NULL)){
      perror("Join error.\n");
    }
  }
  //Join taxi thread after its work has finished
  for (int i = 0; i < num_taxis; i++){
    if(pthread_join(taxiIDs[i], NULL)){
      perror("Join error.\n");
    }
  }
  
  //finalize mutex and semaphore
  pthread_mutex_destroy(&m); 
  sem_close(&full);

  pthread_exit(NULL);
  return 0;
}
