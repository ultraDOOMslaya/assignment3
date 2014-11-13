#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <queue>
#include <vector>
#include <math.h>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

int num_queues=0;
int ageing_time=0;
float scheduled=0;
float wait_time=0;
int slice;

typedef struct process {
  int p_id;
  int burst;
  int burst_time; // original burst for wait_time calculations
  int arrival;
  int priority;
  int absolute; //original priority
  int timeleft; //running count of io
  bool upper;
  int io;
  int trigger_age_up;
}process;

struct node{ //IN ORDER TO UNDERSTAND RECURSION... YOU MUST UNDERSTAND RECURSION...
    int key;
    queue<process> rr;
    unsigned char height;
    node* left;
    node* right;
    node(int k) { key = k; left = right = 0; height = 1; }
};

typedef struct gannt{
  int timespent;
  int p_id;
};

unsigned char height(node* p)
{
    return p?p->height:0;
}

int bfactor(node* p)
{
    return height(p->right)-height(p->left);
}

void fixheight(node* p)
{
    unsigned char hl = height(p->left);
    unsigned char hr = height(p->right);
    p->height = (hl>hr?hl:hr)+1;
}

struct node* findKey(node* tree, int key) {
  if(tree != 0) {
    if(tree->key == key) {
      cout << "matched node: " << tree->key << " with key: " << key << '\n';
      return tree;
    } else if(tree->key > key) {
      cout << "node was greater at: " << tree->key << " than key at: " << key << '\n';
      return findKey(tree->left, key);
    } else {
      cout << "node is less than at: " << tree->key << " with key of: " << key << '\n';
      return findKey(tree->right, key);
    }
  }
} 

process  findMax(queue<process>* run_queue) {
  process start_process;
  for(int i=99; i>0; --i) {
    if(run_queue[i].size() > 0) {
      //cout << "total run queue size is: " << run_queue[i].size() << '\n';
      start_process = run_queue[i].front();
      run_queue[i].pop();
      
      return start_process;
    }
  }
}

node* rotateright(node* p)
{
    node* q = p->left;
    p->left = q->right;
    q->right = p;
    fixheight(p);
    fixheight(q);
    return q;
}

node* rotateleft(node* q)
{
    node* p = q->right;
    q->right = p->left;
    p->left = q;
    fixheight(q);
    fixheight(p);
    return p;
}

node* balance(node* p) // balancing the p node
{
    fixheight(p);
    if( bfactor(p)==2 )
    {
        if( bfactor(p->right) < 0 )
            p->right = rotateright(p->right);
        return rotateleft(p);
    }
    if( bfactor(p)==-2 )
    {
        if( bfactor(p->left) > 0  )
            p->left = rotateleft(p->left);
        return rotateright(p);
    }
    return p; // balancing is not required
}

node* insert(node* p, int k) // insert k key in a tree with p root
{
    if( !p ) return new node(k);
    if( k<p->key )
        p->left = insert(p->left,k);
    else
        p->right = insert(p->right,k);
    return balance(p);
}

node* findmin(node* p) // find a node with minimal key in a p tree 
{
    return p->left?findmin(p->left):p;
}

node* removemin(node* p) // deleting a node with minimal key from a p tree
{
    if( p->left==0 )
        return p->right;
    p->left = removemin(p->left);
    return balance(p);
}

node* remove(node* p, int k) // deleting k key from p tree
{
    if( !p ) return 0;
    if( k < p->key )
        p->left = remove(p->left,k);
    else if( k > p->key )
        p->right = remove(p->right,k);  
    else //  k == p->key 
    {
        node* q = p->left;
        node* r = p->right;
        delete p;
        if( !r ) return q;
        node* min = findmin(r);
        min->right = removemin(r);
        min->left = q;
        return balance(min);
    }
    return balance(p);
}

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
}

void usedUp(vector<process> *done, vector<process> *chunk_to_arrive) {
  for(vector<process>::iterator i=chunk_to_arrive->begin(); i!=chunk_to_arrive->end(); ++i) {  
    done->push_back(*i);
    chunk_to_arrive->erase (chunk_to_arrive->begin());
  }
}
  /* debugging
  int i=0;
  for(i=0; i<processes->size(); i++) {
    cout << "--- curr process and next ---" << '\n';
    cout << (processes->begin())->p_id << '\n';
    cout << (processes->begin()+1)->p_id << '\n';
    cout << "--- ---" << '\n';
  } */
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

int promoProcess(process *p) {
  int pritz = p->priority;
  pritz += 10;
  if(p->upper) {
    if(pritz > 99) {
      pritz = 99;
    }
  } else {
    if(pritz > 49) {
      pritz = 49;
    }
  }
  return pritz;
}

void promo(queue<process>* run_queue, int tick, int curr_priority) {
  process tmp;
  for(int i=curr_priority; i>0; --i) {
    if(run_queue[i].size() > 0) {
      while(run_queue[i].front().trigger_age_up == tick and run_queue[i].size() > 0) {
        //cout << "stuck in promos?" << '\n';
        tmp = run_queue[i].front();
        run_queue[i].pop();
        tmp.priority = promoProcess(&tmp);
        run_queue[tmp.priority].push(tmp);
      }
    }
  }
}

/*void sleuthIO(vector<process> *wait_queue, queue<process>* run_queue ) {
  int count=0;
  //cout << "wait queue length is " << wait_queue.size() << '\n';
  for(vector<process>::iterator i=wait_queue->begin(); i!=wait_queue->end(); ++i) {
    cout << "sleuth?" << '\n';
    if(i->timeleft == 0) {
      cout << "----- from the sleuthing: " << i->timeleft << '\n'; 
      i->priority += i->io;
      if(i->upper) {
        if(i->priority > 99) {
          i->priority = 99;
        }
      } else {
        if(i->priority > 49) {
          i->priority = 49;
        }
      }
      cout << "do you die here?" << '\n';
      run_queue->push(*i);
      cout << "how bout hur?" << '\n';
      i->timeleft = -1; //send it to oblivion 
    } else {
      wait_queue->at(count).timeleft =  i->timeleft - 1;
      cout << "IO: " << i->timeleft << " on " << i->p_id << "'s I/O." << '\n';
    }
    count++;
  }
}*/

void sleuthIO(queue<process>* wait_queue, queue<process>* run_queue) {
  int count=0;
  while(count < wait_queue->size()) {
  //cout << "wait queue size: " << wait_queue->size() << " to count: " << count << '\n';
  process tmp;
  tmp = wait_queue->front();
  wait_queue->pop();
  if(tmp.timeleft == 0) {
      //cout << "----- from the sleuthing: " << tmp.timeleft << '\n'; 
      tmp.priority += tmp.io;
      if(tmp.upper) {
        if(tmp.priority > 99) {
          tmp.priority = 99;
        }
      } else {
        if(tmp.priority > 49) {
          tmp.priority = 49;
        }
      }
      run_queue[tmp.priority].push(tmp);
    } else {
      tmp.timeleft =  tmp.timeleft - 1;
      wait_queue->push(tmp);
      //cout << "IO: " << i->timeleft << " on " << i->p_id << "'s I/O." << '\n';
    }
    ++count; 
  }  
}

void demote(process being_processed, queue<process>* run_queue) {
  //cout << "_______________" << '\n';
  //cout << being_processed.p_id << " was demoted." << '\n';
  //cout << "---------------" << '\n';
  being_processed.priority -= slice;
  if(being_processed.priority < being_processed.absolute) {
    being_processed.priority = being_processed.absolute; 
  }
  being_processed.trigger_age_up += 100;
  run_queue[being_processed.priority].push(being_processed);   
}

void makeChart(vector<gannt> chart) {
  
}

int main() {

  //int scheduled;
  //int wait_time;
  bool start = true;
  bool queued = false;
  cout << "Enter a time slice: " << '\n';
  cin >>  slice;
  int processor_slice=0;
  bool processing = false;
  bool pending_arrival = true;
  int tick=0;
  process being_processed; // a tmp process we use in multiple spots
  vector<process> arrivals;
  //vector<process> wait_queue;
  queue<process> wait_queue;
  //vector<queue<process> > run_queue;
  queue<process> run_queue[100];
  int process_count=0;
  struct node *rr_tree;
  struct node *curr_node;
  struct root *head;
  int not_processing=0;
  int curr_priority=0;
  int wait_time=0;
  int avg_wait_time=0;
  int total_process;
  int turnaround=0;
  int NP=-1;
  vector<gannt> chart;
  gannt cycle;

  //fstream myfile ("100k_processes"); 
  int count=0;
  string buf;
  string line;
  int c=0;
  
  string infile;
  cout << "Enter the name of the file: " << '\n';
  cin >> infile;
  fstream myfile (infile.c_str());
 
  if (myfile.is_open()) {
    while (!myfile.eof()) {
      getline (myfile,line);
      stringstream ss(line);
      string tokens[6];
      count=0;
      c++;
      while (ss >> buf) {
        tokens[count] = buf;
        //cout << buf << '\n';
        count++;
      }
      if(c > 1) {
        being_processed.p_id=atoi(tokens[0].c_str());
        being_processed.burst=atoi(tokens[1].c_str());
        being_processed.burst_time=atoi(tokens[1].c_str());
        being_processed.arrival=atoi(tokens[2].c_str());
        being_processed.priority=atoi(tokens[3].c_str());
        being_processed.io=atoi(tokens[5].c_str()); 
        being_processed.absolute=atoi(tokens[3].c_str());
        being_processed.trigger_age_up=0;
        ++process_count;
        //cout << being_processed.p_id << " ";
        
      }
      if(being_processed.p_id<0){
        being_processed.p_id *= -1;
        //cout << being_processed.p_id << '\n';
      }
      if(being_processed.burst<0){
        being_processed.burst *= -1;
        //cout << being_processed.p_id << '\n';
      }
      if(being_processed.burst_time<0){
        being_processed.burst_time *= -1;
        //cout << being_processed.p_id << '\n';
      }
      if(being_processed.arrival<0){
        being_processed.arrival *= -1;
      }
      if(being_processed.priority<0){
        being_processed.priority *= -1;
      }
      if(being_processed.io<0){
        being_processed.priority *= -1;
      }
      if(being_processed.absolute<0){
        being_processed.priority *= -1;
      }
      arrivals.push_back(being_processed);
      //cout << being_processed.p_id << " was added" << '\n';
    }
  } else cout << "Unable to open file"; 
  //cout << '\n';
  
  total_process = process_count;
  /*cout << "processes: " << '\n';
  */
  //array to hold our ready queues

  /*process p0 = {1, 8, 0, 23, 8, 0};
  process p1 = {2, 4, 34, 17, 4, 0};
  process p2 = {3, 109, 2, 5, 109, 0};
  process p3 = {4, 200, 15, 3, 200, 0};
  process p4 = {5, 130, 15, 8, 130, 0};
  
  arrivals.resize(5);
  arrivals[0] = p0;
  arrivals[1] = p1;
  arrivals[2] = p2;
  arrivals[3] = p3;
  arrivals[4] = p4;
  process_count = 5;*/
  sort (arrivals.begin(), arrivals.end(), myfunction);
  vector<process>::iterator iter=arrivals.begin();
  vector<process> chunk_to_arrive;
  /*for(vector<process>::iterator i=arrivals.begin(); i<arrivals.end(); ++i) {
    cout << i->p_id << '\n';
    cout << "and io " << i->io << '\n';
  }*/
  
  //make 100 nodes with 100 queues
 /* for(int num_nodes=0; num_nodes<100; ++num_nodes) {
    insert(rr_tree, num_nodes);
    cout << "inserted: " << num_nodes << '\n';
    cout << "head is: " << rr_tree->key << '\n';
  }*/
  //process_count = process_count-2;    
  while(process_count != 0) {
    //cout << "amount of processes to process left: " << process_count << '\n';
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

    //cout << "about to pend" << '\n';
    if(pending_arrival) {
      for(vector<process>::iterator it=chunk_to_arrive.begin(); it!=chunk_to_arrive.end(); ++it) {
        //cout << "get here1" << '\n';
        if(it->absolute > 49) {
          it->upper = true;
        }
        NP = NP + 1;
        run_queue[it->priority].push(*it);
        //cout << "key for item just added to queue at leaf node: " << '\n';
        //cout << rr_tree->rr.front().p_id << '\n';
      } 
      //cout << "clock tick when we push to the queue: " << tick << '\n';
      //cout << "_________________" << '\n';
      chunk_to_arrive.clear();
    }
      
    //if tree isn't empty start processing
    if(pending_arrival) {
      if(!processing and !queued) {
        //cout << "about to set a process" << '\n';
        //find the highest priority with a size > 0  
        //cout << "get b" << '\n';
        being_processed = findMax(run_queue);
        curr_priority = being_processed.priority;
        not_processing = -8;
        processing = true;
        processor_slice = 0;
        cycle.p_id = being_processed.p_id;
        //cout << being_processed.p_id << '\n'; 
      }
    } else if(!processing and queued) {
        //cout << "logic error?" << '\n';
       if(run_queue->size() > 0) {
           processing = true;
           being_processed = findMax(run_queue);
           not_processing = -8;
           processing = true;
           processor_slice = 0;
           cycle.p_id = being_processed.p_id;
       } 
       queued = false;
    } else if(!processing and (run_queue->size() > 0)) {
        being_processed = findMax(run_queue);
        curr_priority = being_processed.priority;
        not_processing = -8;
        processing = true;
        processor_slice = 0; 
        cycle.p_id = being_processed.p_id;
    }
    
      //
    if(processing) {
      //cout << "being processed: " << being_processed.p_id << " with burst of... " << being_processed.burst << " at tick: " << tick << '\n'; 
      if(processor_slice == slice) {
        // check for clock interrupt
        if(being_processed.burst > 0) {
          demote(being_processed, run_queue);
          processing = false;
          chart.push_back(cycle);
          cycle.timespent=0;
        }
      } else if(processor_slice == (slice-1)) {
        // check for io
        if(being_processed.io > 0) {
          being_processed.timeleft = being_processed.io;
          wait_queue.push(being_processed);
          queued = true;
          processing = false;
          chart.push_back(cycle);
          cycle.timespent=0;
        }  
      } else {
        --being_processed.burst;
        cycle.timespent++;
        //continue processing or finish when burst is 0
        if(being_processed.burst <= 0) {
          processing = false;
          --process_count;
          chart.push_back(cycle);
          cycle.timespent=0;
          if(being_processed.p_id > 0) {
            wait_time = wait_time + ((tick - being_processed.arrival) - being_processed.burst_time);
            
            turnaround = turnaround + (tick - being_processed.arrival);
          }
          //cout << "being processed: " << being_processed.p_id << " finished at clock tick: " << tick << '\n'; 
          //cout << "processes left are: " << process_count << '\n';
        }
      }
      ++processor_slice;
    }

    if(wait_queue.size() > 0) {
      //cout << "wakw waka" << '\n';
      //cout << "wait queue size? " << wait_queue.size() << '\n';
      sleuthIO(&wait_queue, run_queue);
    }

    //cout << "still looping?" << '\n';
    if(not_processing < 99) {
      promo(run_queue, tick, curr_priority);
    }
    /*if(not_processing > 5000) {
      cout << "we are stuck and need help..." << '\n';
      cout << "--------------" << '\n';
      cout << "size of the run queue... " << run_queue->size() << '\n';
      cout << "size of the wait queue... " << wait_queue.size() << '\n';
    }*/
    ++not_processing;
    ++tick;
  }
  cout << "We done foo, total ticks are: " << tick << '\n';
  cout << '\n';
  cout << "----- GANNT CHART -----" << '\n';
  makeChart(chart);
  cout << "Average wait time is:       " << (wait_time/total_process) << '\n';
  cout << "Average turnaround time is: " << (turnaround/total_process) << '\n';
  cout << "NP is:                      " << NP << '\n';
}
