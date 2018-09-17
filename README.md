# Operating_Systems_Project
OS Final project 
The final project for operating systems was completed by my self and two other students. The project demonstates succesful implementation of a multithreaded quicksort using Posix pthreads and Linux OS systems calls. The following is the report which accompanied the submission:

QuickSort Implemented with Multithreaded Programming
Authors: David Decker, Amir Afridi, Justin Sanchez, Thomas Pannozzo
Abstract
In this project we attempted to implement the QuickSort algorithm using multithreaded
programming in C++17. Doing so dramatically increased performance of this algorithm by speeding up
sort times versus the original QuickSort algorithm.
Introduction
The QuickSort algorithm is already one of the fastest sorting algorithms as it’s name implies but
we wanted to make it even faster. By implementing this algorithm with multithreaded programming we
hoped to achieve faster sort times since the recursive calls would be split into separate threads. Each
recursive call creates new threads for the upper and lower bound of the array; this continues with each
recursive call until the program reaches the maximum number of threads in which case it just finishes the
sort by calling the normal quicksort.
Methodology
We began this project by getting a running quicksort algorithm. For this we needed partition and
swag algorithms in order to implement quicksort. Located below is the code for these algorithms:
Partition
1. int partition (int low, int high)
2. {
3. // pivot (Element to be placed at right position)
4. int pivot = programArray[high];
5.
6. int i = (low - 1); // Index of smaller element
7.
8. for (int j = low; j <= high- 1; j++) {
9. // If current element is smaller than or
10. // equal to pivot
11. if (programArray[j] <= pivot) {
12. i++; // increment index of smaller element
13. swap(programArray[i], programArray[j]);
14. }
15. }
16. swap (programArray[i + 1], programArray[high]);
17.Swap
1. void swap(int* a, int* b)
2. {
3. int t = *a;
4. *a = *b;
5. *b = t;
6. }
Quicksort
1. void quicksort(void *voidBounds){
2.
3. int* bounds = (int *) voidBounds;
4.
5. if (bounds[LOW] < bounds[HIGH]) {
6. // pi is partitioning index, arr[p] is now at right place
7. int partitionIndex = partition(bounds[LOW], bounds[HIGH]);
8.
9. int lowBounds[2] = {bounds[LOW], partitionIndex - 1};
10. int highBounds[2] = {partitionIndex + 1, bounds[HIGH]};
11.
12. quicksort(lowBounds);
13. quicksort(highBounds);
14. }
15. }
Once these algorithms were up and running we had our starting point. We were able to gather
data and see average run times for data sets ranging from very small to very large. Having this
information allowed us to see how large of an improvement our multithreading would provide. We decided
that after each recursive call it would make the most sense for a new thread to be created for the low end
and the high end of the sort. This continues until all possible threads are created at which point the
regular quicksort is called to finish off the recursive sorting that is occurring in each thread. Located below
is the code for our implementation of a threaded quicksort:
Threaded Quicksort
1. void *threadedQuicksort(void *voidBounds){
2.
3. int* bounds = (int *) voidBounds; // pthread creation REQUIRES a void pointer, cast this to an
int so we can use it
4.
5. if (bounds[LOW] < bounds[HIGH]){
6.
7. pthread_t lowRecurringThread[1]; // Create threads for low/high recurs of quicksort
8. pthread_t highRecurringThread[1];
9.
10. pthread_attr_t attr[1]; // Cargo-culted pthread overhead
11. fflush(stdout);
12. pthread_attr_init(&attr[0]);
13. pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
14.
15.
16. int partitionIndex = partition(bounds[LOW], bounds[HIGH]); // Call partition algorithm, acquire
new index
17.
18. int lowBounds[3] = {bounds[LOW], partitionIndex - 1, bounds[2]+1}; // Acquire new bounds
to send to two threads
19. int highBounds[3] = {partitionIndex + 1, bounds[HIGH], bounds[2]+1};
20.
21. if (bounds[2] < 14){
22. std::cout << "DEPTH: " << bounds[2] << std::endl;
23. pthread_create(&lowRecurringThread[0], &attr[0], threadedQuicksort, lowBounds); //
Send a thread to quicksort above/below the partition
24. pthread_create(&highRecurringThread[0], &attr[0], threadedQuicksort, highBounds);
25. pthread_join(lowRecurringThread[0], NULL); // Join the threads after completion
26. pthread_join(highRecurringThread[0], NULL);
27. }
28. else {
29. quicksort(lowBounds);
30. quicksort(highBounds);
31. }
32. }
33.
34. return voidBounds; // Garbage return value
35. }
Results
For us to measure our results we had to use functions to record our starting and ending time. This
was done using C++ libraries to help with recording of time. We found that with a random array the
regular quicksort was slightly faster but in a sorted and reverse sorted array the threaded quicksort was
significantly faster.
Threaded Quicksort Regular Quicksort
Random Array .0344 seconds .0208 seconds
Sorted Array .1839 seconds .5642 seconds
Reverse Sorted Array .0445 seconds .0611 seconds
Conclusions
Implementing quicksort using multithreading has shown us the power of multithreaded
programming. We learned that all computers have a maximum amount of threads they can run without
hurting performance and we had to figure this out by trial and error until we researched more into the
idea.
