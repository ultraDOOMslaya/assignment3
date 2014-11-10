#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <queue>
#include <vector>

using namespace std;

int QUANTUM=100;

typedef struct process {
  int p_id;
  int burst;
  int arrival;
  int priority;
  int timeleft;
}process;

typedef struct rr_queue {
  queue<process> rr;
  int quantum;
}rr_queue;

typedef struct fcfs_queue {
  queue<process> fcfs;
  int quantum;
}fcfs_queue;

/*
0 Number of queues:
variable, upper bound = 5 (ask user to input number)

1 Scheduling algorithms for each queue:
a. Round Robin for all except last queue (FCFS).
b. Time quantum: doubled for each subsequent queue below it.

2 Method used to determine when to demote a process:
Processes that used up their time quantum and still cannot complete are demoted.

3 Ageing: when a process waits in a queue for more than some specified value (value is prompted). Apply this all but the first queue.
*/

//partition all the processes with the same arrival time from the master vector to be fed into the ready queue
void steal(vector<process> *processes, vector<process> *chunk_to_arrive) {
  int set_point = processes->begin()->arrival;
  int count = 0;

  for(vector<process>::iterator i=processes->begin(); i!=processes->end(); ++i) {
    //push every process that has the same arrival time as the first process in the vector... delete these from the vector
    while((i->arrival == set_point) and !processes->empty()) {
      chunk_to_arrive->push_back(*i);
      processes->erase (processes->begin());
    }        
    break;
  }
  /* debugging
  int i=0;
  for(i=0; i<processes->size(); i++) {
    cout << "--- curr process and next ---" << '\n';
    cout << (processes->begin())->p_id << '\n';
    cout << (processes->begin()+1)->p_id << '\n';
    cout << "--- ---" << '\n';
  } */
}

//method for sorting process structs based on arrival time
static bool myfunction (const process &x, const process &y) { return (x.arrival < y.arrival); }

//method for sorting process structs (about to be pushed to ready queue) by their priority or p_id (Dr. Stevenson would not like this method!)
static bool sortArrivals (const process &x, const process &y) { 
  if (x.priority != y.priority) {
    return (x.priority < y.priority);
  } else {
    return (x.p_id < y.p_id);
  }
}

//schedule the arrivals to feed to the first rr_queue (array sorted by arrival time)
void organizeArrivals(vector<process> *chunk_to_arrive) {
  sort (chunk_to_arrive->begin(), chunk_to_arrive->end(), sortArrivals);
}

int main() {

  int slice = 8;

  rr_queue q1; q1.quantum = slice;
  rr_queue q2; q2.quantum = (slice*2);
  fcfs_queue q3; q3.quantum = (slice*3); 

  //array to hold our ready queues
  rr_queue queues[2] = {q1, q2};

  process p0 = {1, 8, 0, 23, 8};
  process p1 = {2, 4, 34, 17, 4};
  process p2 = {3, 56, 2, 5, 12};
  process p3 = {4, 32, 15, 3, 12};
  process p4 = {5, 44, 15, 8, 10};
  
  vector<process> arrivals(5);
  arrivals[0] = p0;
  arrivals[1] = p1;
  arrivals[2] = p2;
  arrivals[3] = p3;
  arrivals[4] = p4;

  sort (arrivals.begin(), arrivals.end(), myfunction);
  vector<process> chunk_to_arrive;

  //while master vector has still has processes to be pushed to the ready queue
  while(arrivals.size() != 0) {
    cout << arrivals.size() << '\n';
    steal(&arrivals, &chunk_to_arrive);
    organizeArrivals(&chunk_to_arrive);
     
    for(vector<process>::iterator it=chunk_to_arrive.begin(); it!=chunk_to_arrive.end(); ++it) {
      cout << "p_id is: " << it->p_id << '\n';
    }
    cout << '\n';
    chunk_to_arrive.clear();
    cout << "size of arrival after clearing chunk: " << arrivals.size() << '\n';
    //cout << "im stuck" << '\n';
  }
  cout << "we done foo" << '\n';

}
