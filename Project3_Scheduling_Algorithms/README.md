This project involves implementing several different process scheduling algorithms.
The scheduler will be assigned a predefined set of tasks and will schedule the tasks based on the selected scheduling algorithm. 
Each task is assigned a priority and CPU burst. The following scheduling algorithms will be implemented:
1. First-come, first-served (FCFS), which schedules tasks in the order in which they request the CPU.
2. Shortest-job-first (SJF), which schedules tasks in order of the length of the tasks' next CPU burst.
3. Priority scheduling, which schedules tasks based on priority.
4. Round-robin (RR) scheduling, where each task is run for a time quantum (or for the remainder of its CPU burst).
5. Priority with round-robin, which schedules tasks in order of priority and uses round-robin scheduling for tasks with equal priority.


Completing this project will require writing the following C files:

schedule_fcfs.c
schedule_sjf.c
schedule_rr.c
schedule_priority.c
schedule_priority_rr.c

The supporting files invoke the appropriate scheduling algorithm. 

For example, to build the FCFS scheduler, enter

make fcfs

which builds the fcfs executable file.
