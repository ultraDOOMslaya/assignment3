#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <queue>
#include <vector>

using namespace std;

int num_queues=0;
int ageing_time=0;

typedef struct process {
  int p_id;
  int burst;
  int arrival;
  int priority;
  int timeleft;
  int trigger_age_up;
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

//method for ageing the entire data structure(s) (need to rewrite because ageing happens everywhere cept queue 0)
void ageing(rr_queue *queues, fcfs_queue *final, int tick) {
  process tmp;
  int length = queues->rr.size();
  while(final->fcfs.front().timeleft == tick) {
    memcpy(&final->fcfs.front(), &tmp, sizeof final->fcfs);
    final->fcfs.pop();
    queues[length].rr.push(tmp);
  }
}

//method for demoting a process
void demote(rr_queue *queues, fcfs_queue *final, int tick, int que_val, process *being_demoted) {
  cout << "I demoted process: " << being_demoted->p_id << '\n';
  //process tmp = queues[que_val].rr.front();
  //queues[que_val].rr.pop();
  //tmp.trigger_age_up = tick + ageing_time;

  //check if process is demoted to fcfs queue
  if((que_val+1) == num_queues) {
    final->fcfs.push(*being_demoted);
  } else {
    queues[que_val+1].rr.push(*being_demoted);
  }
}

//debugging method... don't use this it actually breaks something
void checkQueue(rr_queue *queues) {
  process p;
  cout << "heeeello? " << queues->rr.size() << '\n';
  for(int i=0; i<queues->rr.size(); ++i) {
    p = queues[i].rr.front();
    cout << "p id of next item in this queue is: " <<  p.p_id << '\n';
    queues[i].rr.pop();
  }
}

int main() {

  int slice = 8;
  int processor_slice=0;
  bool processing = false;
  bool pending_arrival = true;
  int curr_queue=0;
  int tick=0;
  process being_processed;

  rr_queue q1; q1.quantum = slice;
  rr_queue q2; q2.quantum = (slice*2);
  fcfs_queue q3; q3.quantum = (slice*3);
  num_queues = 2;
  ageing_time = 50;
   

  //array to hold our ready queues
  rr_queue queues[2] = {q1, q2};

  process p0 = {1, 8, 0, 23, 8, 0};
  process p1 = {2, 4, 34, 17, 4, 0};
  process p2 = {3, 56, 2, 5, 56, 0};
  process p3 = {4, 32, 15, 3, 32, 0};
  process p4 = {5, 44, 15, 8, 44, 0};
  
  vector<process> arrivals(5);
  arrivals[0] = p0;
  arrivals[1] = p1;
  arrivals[2] = p2;
  arrivals[3] = p3;
  arrivals[4] = p4;
  int process_count = 5;
  int apple=0;
  sort (arrivals.begin(), arrivals.end(), myfunction);
  vector<process> chunk_to_arrive;

  //while master vector still has processes to be pushed to the ready queue
  while(process_count != 0) {
    //cout << "am i stuck?" << '\n';
    if(arrivals.size() != 0) {
      if(arrivals.begin()->arrival == tick) {
        pending_arrival = true;
        steal(&arrivals, &chunk_to_arrive);
        organizeArrivals(&chunk_to_arrive);
      } else {
        pending_arrival = false;
      }
    } else if(arrivals.size() == 0 and pending_arrival == true) {
      pending_arrival = false;
    }
    
    //push the stuff thats arrived to the ready_queue
    if(pending_arrival) {
      for(vector<process>::iterator it=chunk_to_arrive.begin(); it!=chunk_to_arrive.end(); ++it) {
        cout << "what were pushin... " << it->p_id << '\n';
        queues[0].rr.push(*it);
        cout << "clock tick when we push to the queue: " << tick << '\n';
      }
      chunk_to_arrive.clear();    
    }

    //if the process is free:
    //cout << "status of processing: " << processing << '\n';
    if(!processing and process_count != 0) {
      cout << "about to enter the VOID1 " << queues[0].rr.front().p_id << '\n';
      cout << "clock tick: " << tick << '\n';
      //cout << "processor count is: " << process_count << '\n';
      //age up
      //check every queue for a process... execute the first one we find
      curr_queue=0;
      int temp=0;
      bool stop=false;
      cout << "about to enter the VOID2 " << queues[0].rr.front().p_id << '\n';
      //checkQueue(queues);  
      do {
        cout << "about to enter the VOID3 " << queues[0].rr.front().p_id << '\n'; 
        if(queues[temp].rr.size() > 0) {
          cout << "wormhole..." <<  '\n';
          processing = true;
          curr_queue = temp;
          being_processed = queues[curr_queue].rr.front();
          cout << "lol " << queues[curr_queue].rr.front().p_id << '\n';
          cout << "being_processed found: " << being_processed.p_id << '\n';
          queues[curr_queue].rr.pop();
        }
        ++temp;
        //cout << "stevenson hates do while loops" << '\n';
      } while((temp<(num_queues-1)) or processing != true);
      //cout << "did i make it out?" << '\n';
      if(!processing) {
        //cout << "hell ya i did!" << '\n';
        curr_queue = num_queues;
        processing = true;
        being_processed = queues[curr_queue].rr.front();
        queues[curr_queue].rr.pop();
      }
      //stop ageing for the executed process
      cout << "the first choosen p-id is: " << being_processed.p_id << '\n';
      being_processed.trigger_age_up = -(slice*(curr_queue+1));
      processor_slice=0;
    }

    //if the processor is occupied: check if its quantum is up and tick its current process down
    else {
      if(apple < 20) {
        cout << "p_id of front of queue 1 for 20 ticks: " << queues[0].rr.front().p_id << " at tick: " << tick << '\n';
        ++apple;
      }
      //cout << "how many times do i get run? (should be 5)" << '\n';
      //tickle the current process:
      //cout << "actually being processed: " << being_processed.p_id << '\n';
      being_processed.trigger_age_up += 1;
      being_processed.timeleft = being_processed.timeleft - 1;
      ++processor_slice;
      if(processor_slice == slice*curr_queue) {
        //age up
        //demote
        cout << "demoted: " << being_processed.p_id << '\n';
        cout << "front of queue before demotion... " << queues[0].rr.front().p_id << '\n'; 
        demote(queues, &q3, tick, curr_queue, &being_processed);
      }
      //cout << being_processed.timeleft << '\n';
      if(being_processed.timeleft == 0) {
        cout << "ever true?" << '\n';
        processing=false;
        --process_count;
      }
    }

    /* for maximum frustration... leave this FUCKING block of code uncommented while debugging!
    int h=0; 
    for(h=0; h<queues[0].rr.size(); ++h) {
      cout << queues[0].rr.front().p_id << '\n';
      queues[0].rr.pop();
    }*/
    //increment tick...
    ++tick;
  }
  cout << "We done foo, total ticks are: " << tick << '\n';
}
