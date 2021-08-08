#include <string.h>
#include <stdlib.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"


// array of lists for each priority for the first insertion
struct node *list[MAX_PRIORITY + 1];
// array of lists for each priority for the second insertion in the right order 
struct node *list2[MAX_PRIORITY + 1];
// node for the traking each priority list
struct node *i_node;

// in this file this functions used for the reordering to the right list 
bool comesBefore(char *a, char *b) { return strcmp(a, b) > 0; }

// add funtion to declare all properties for the task and insert it into the
// array of list with index of priority
void add(char *name, int priority, int burst) {
  Task *t = malloc(sizeof(Task));
  t->name = name;
  t->priority = priority;
  t->burst = burst;
  insert(&list[priority], t);
}

// function for reordering to new list
Task *pickNextTask(struct node *i_list) {
  Task *first_by_name = i_list->task;
  struct node *current_node = i_list;
  // while loop for taking a next task based on name
  while (current_node) {
    if (comesBefore(current_node->task->name, first_by_name->name)) {
      first_by_name = current_node->task;
    }
    current_node = current_node->next;
  }
  return first_by_name;
}

// function for updating each list for particular priority when it ends
Task *pickNextTask2(struct node *i_list) {
  Task *ret = i_node->task;
  if (i_node->next)
    i_node = i_node->next;
  else
    i_node = i_list;
  return ret;
}

void schedule() {
  // put tasks from list to list2 in the proper order with pparticular priority
  for (int i = MAX_PRIORITY; i >= MIN_PRIORITY; --i) {
    while (list[i]) {
      Task *n = pickNextTask(list[i]);
      insert(&list2[i], n);
      delete (&list[i], n);
    }
  }

  // running all tasks until all lists are empty
  for (int i = MAX_PRIORITY; i >= MIN_PRIORITY; --i) {
    i_node = list2[i];
    // running each separate list until it empty
    while (list2[i]) {
      Task *t = pickNextTask2(list2[i]);
      // if it is the only node for this priority run it all together without
      // separation
      if (list2[i]->next == NULL) {
        run(t, t->burst);
        break;
      }
      // if cpu burst - quantum less or equal a zero we are running it and
      // delting
      // it then
      if (t->burst - QUANTUM <= 0) {
        run(t, t->burst);
        delete (&list2[i], t);
      }
      // if not cpu burst time equal a quantum and we not delete a task
      else {
        run(t, QUANTUM);
        t->burst = t->burst - QUANTUM;
      }
    }
  }
}