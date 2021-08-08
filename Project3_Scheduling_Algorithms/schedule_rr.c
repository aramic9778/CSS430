#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"


// main list where we add tasks from the text file
struct node *list = NULL;
// list for the traking a current node
struct node *current_node;
// list for the right order
struct node *new_node;

// in this file this functions used for the reordering to the right list
bool comesBefore(char *a, char *b) { return strcmp(a, b) > 0; }

// add funtion to declare all properties for the task and insert it into the
// list
void add(char *name, int priority, int burst) {
  Task *temp = malloc(sizeof(Task));
  temp->name = name;
  temp->priority = priority;
  temp->burst = burst;
  insert(&list, temp);
}

// function for reordering to new list
Task *pickNextTask() {
  Task *first_by_name = list->task;
  struct node *current_node = list;
  // while loop for taking a next task based on name
  while (current_node) {
    if (comesBefore(current_node->task->name, first_by_name->name)) {
      first_by_name = current_node->task;
    }
    current_node = current_node->next;
  }
  return first_by_name;
}
// function for updating a list when it ends
Task *pickNextTask2() {
  Task *first_task = current_node->task;
  if (current_node->next)
    current_node = current_node->next;
  else
    current_node = new_node;
  return first_task;
}

void schedule() {
  // put tasks from list to new_node in the proper order
  while (list) {
    Task *tt = pickNextTask();
    insert(&new_node, tt);
    delete (&list, tt);
  }

  current_node = new_node;
  // running all tasks until new_node list is empty
  while (new_node) {
    Task *temp2 = pickNextTask2();
    // if cpu burst - quantum less or equal a zero we are running it and deltinf
    // it then
    if (temp2->burst - QUANTUM <= 0) {
      run(temp2, temp2->burst);
      delete (&new_node, temp2);
    }
    // if not cpu burst time equal a quantum and we not delete a task
    else {
      run(temp2, QUANTUM);
      temp2->burst = temp2->burst - QUANTUM;
    }
  }
}