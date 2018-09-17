//Amir Afridi, David Decker, Justin Sanchez, Thomas Pannozzo
//COMPILE LINE: module load compilers/gcc/6.2.0
//              g++ -std=gnu++17 -g -Wall -o qs quicksort.cpp -lpthread -lrt
 
 
 
//Some of these are obsolete, pulled this straight from my ringbuffer, trim this later
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sys/time.h>
#include <math.h>
 
using namespace std;
 
#define _REENTRANT
#define MAX_ARR_SIZE 100000
 
bool LOW = 0;
bool HIGH = 1;
 
void *threadedQuicksort(void *bounds);  // Two quicksorts to compare time differences, implement a timer later
void quicksort(void *bounds);
int partition (int low, int high);
void swap(int* a, int* b);
void fillArray(string a);
timespec startTime();
timespec endTime();
void totalTime(timespec start, timespec end);
void printTime(timespec time);

 
int * programArray; //Global array to get around only having one argument for our pthreads
 
int main(){
 
    // Calls the fill array function
    fillArray("random");
    int bounds[3] = {0, MAX_ARR_SIZE, 0};   // Initial call, low is 0, high is end of array, third mem is depth (for pthreads)
	cout << "first random threaded quicksort" << endl;
	timespec start = startTime();

    threadedQuicksort(bounds);
	
	timespec end = endTime();
	totalTime(start, end);

    cout << "now random quicksort" << endl;
	fillArray("random");
	bounds[0] = 0;
	bounds[1] = MAX_ARR_SIZE;
	bounds[2] = 0;
	start = startTime();

	quicksort(bounds);

	end = endTime();
	totalTime(start, end);	
	
	cout << "now in order threaded quicksort" << endl;
	fillArray("order");
	bounds[0] = 0;
	bounds[1] = MAX_ARR_SIZE;
	bounds[2] = 0;

    start = startTime();

    threadedQuicksort(bounds);
	
	end = endTime();
	totalTime(start, end);

    cout << "now in order quicksort" << endl;
	fillArray("order");
	bounds[0] = 0;
	bounds[1] = MAX_ARR_SIZE;
	bounds[2] = 0;
	
	start = startTime();

	quicksort(bounds);

	end = endTime();
	totalTime(start, end);	
	
	cout << "finally reverse order threaded quicksort" << endl;
	
	fillArray("reverse");
    bounds[0] = 0;
	bounds[1] = MAX_ARR_SIZE;
	bounds[2] = 0;
	
	start = startTime();

    threadedQuicksort(bounds);
	
	end = endTime();
	totalTime(start, end);
	
	cout << "now reverse quicksort" << endl;
    fillArray("reverse");
    bounds[0] = 0;
	bounds[1] = MAX_ARR_SIZE;
	bounds[2] = 0;
	
	start = startTime();
	
	quicksort(bounds);

	end = endTime();
	totalTime(start, end);	

	free(programArray);
    return 0;
}
 
// Regular quicksort
void quicksort(void *voidBounds){
 
    int* bounds = (int *) voidBounds;
   
    if (bounds[LOW] < bounds[HIGH]) {
        // pi is partitioning index, arr[p] is now at right place
        int partitionIndex = partition(bounds[LOW], bounds[HIGH]);
 
        int lowBounds[2] = {bounds[LOW], partitionIndex - 1};
        int highBounds[2] = {partitionIndex + 1, bounds[HIGH]};
       
        quicksort(lowBounds);
        quicksort(highBounds);
    }
	else
		return;
}
 
// Quicksort using threads
void *threadedQuicksort(void *voidBounds){
 
    int* bounds = (int *) voidBounds;   // pthread creation REQUIRES a void pointer, cast this to an int so we can use it
 
    if (bounds[LOW] < bounds[HIGH]){
 
        pthread_t lowRecurringThread[1];    // Create threads for low/high recurs of quicksort
        pthread_t highRecurringThread[1];
 
        pthread_attr_t attr[1];             // Cargo-culted pthread overhead
        fflush(stdout);
        pthread_attr_init(&attr[0]);
        pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
 
 
        int partitionIndex = partition(bounds[LOW], bounds[HIGH]);  // Call partition algorithm, acquire new index
 
        int lowBounds[3] = {bounds[LOW], partitionIndex - 1, bounds[2]+1};       // Acquire new bounds to send to two threads
        int highBounds[3] = {partitionIndex + 1, bounds[HIGH], bounds[2]+1};
           
        if (bounds[2] <8){
            //std::cout << "DEPTH: " << bounds[2] << std::endl;
            pthread_create(&lowRecurringThread[0], &attr[0], threadedQuicksort, lowBounds);     // Send a thread to quicksort above/below the partition
            pthread_create(&highRecurringThread[0], &attr[0], threadedQuicksort, highBounds);
            pthread_join(lowRecurringThread[0], NULL);  // Join the threads after completion
            pthread_join(highRecurringThread[0], NULL);
        }
        else {
            quicksort(lowBounds);
            quicksort(highBounds);
        }
    }
 
    return voidBounds;  // Garbage return value
}
 
 
// Partition algorithm for both quicksorts
int partition (int low, int high)
{
    // pivot (Element to be placed at right position)
    int pivot = programArray[high];
 
    int i = (low - 1);  // Index of smaller element
 
    for (int j = low; j <= high- 1; j++) {
        // If current element is smaller than or
        // equal to pivot
        if (programArray[j] <= pivot) {
            i++;    // increment index of smaller element
            swap(programArray[i], programArray[j]);
        }
    }
    swap (programArray[i + 1], programArray[high]);
    return (i + 1);
}
 
void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
 
//Fills array with random values between 0 and RAND_MAX (minimum of 32767 to ????)
void fillArray(string a){
    programArray = (int *) malloc(sizeof(int) * MAX_ARR_SIZE);
    if (a == "random") { // random array
        for (int i = 0; i < MAX_ARR_SIZE; i++){
            programArray[i] = rand();
        }
       
    } else if (a == "order") { // in order array
        for (int i = 0; i < MAX_ARR_SIZE; i++) {
            programArray[i] = i;
        }
    } else if (a == "reverse") { // reverse ordered array
        for (int i = 0; i < MAX_ARR_SIZE; i++) {
            programArray[i] = MAX_ARR_SIZE - i;
        }
    }
 
    return;
 
}

timespec startTime()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	//printf("Start time is: %.9ld \n", time.tv_nsec);
	return time;
}

timespec endTime()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	//printf("End time is: %.9ld \n", time.tv_nsec);
	return time;
}

void totalTime(timespec start, timespec end)
{
	double total = (double)end.tv_nsec - (double)start.tv_nsec;
    printf("Total time: %.10e nanoseconds \n", fabs(total));
}

void printTime(timespec time)
{
	printf("%.9ld", time.tv_nsec);
}
