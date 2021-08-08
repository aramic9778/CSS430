#include <stdbool.h>

#define MIN_PRIORITY 1
#define MAX_PRIORITY 40 // I changed max priority to 40 to get output for rr-schedule.txt as well 

// add a task to the list 
void add(char *name, int priority, int burst);

// invoke the scheduler
void schedule();
// function for comparison
bool comesBefore(char *a, char *b);
