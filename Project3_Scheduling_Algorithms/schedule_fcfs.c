#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"



// main list where we will insert all the nodes
struct node *list = NULL;

// function to check the next item
bool comesBefore(char *a, char *b) { return strcmp(a, b) < 0; }

// add funtion to declare all properties for the task and insert it into the
// list
void add(char *name, int priority, int burst) {
  Task *temp = malloc(sizeof(Task));
  temp->name = name;
  temp->priority = priority;
  temp->burst = burst;
  insert(&list, temp);
}

// function to pick the next task for running
Task *pickNextTask() {
  Task *first_by_name = list->task;
  struct node *current_node = list;

  // using a while loop to get the task based on the name
  while (current_node) {
    if (comesBefore(current_node->task->name, first_by_name->name)) {
      first_by_name = current_node->task;
    }
    current_node = current_node->next;
  }
  return first_by_name;
}


// schedule function for running all tasks and deleting them after
void schedule() {
  while (list) {
    Task *temp = pickNextTask();
    run(temp, temp->burst);
    delete (&list, temp);
  }
}