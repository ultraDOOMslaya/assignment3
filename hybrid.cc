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

typedef struct process {
  int p_id;
  int burst;
  int arrival;
  int priority;
  int absolute;
  int timeleft;
  int io;
  int trigger_age_up;
}process;

struct node{
    int key;
    queue<process> rr;
    unsigned char height;
    node* left;
    node* right;
    node(int k) { key = k; left = right = 0; height = 1; }
};

struct root{
  node* top;
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
node* findKey(node* tree, int key) {
  bool found = false;
  if(tree != NULL) {
    while(!found) {
      if(tree->key > key) {
        tree = tree->left;
      } else if(tree->key < key) {
        tree = tree->right;
      } else if(tree->key == key) {
        found = true;
      }
    }
  return tree; 
}

node* findMax(node* max_node) {
  if(max_node != NULL) {
    while(max_node->right != NULL) {
      max_node = max_node->right;
    }
  }
  return max_node;
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
    if( !p ) {
      node* new_node = new node(k);
      return new_node;
    }
    if( k<p->key )
        p->left = insert(p->left,k,pr);
    else
        p->right = insert(p->right,k,pr);
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

void sleuthIO(vector<process> *wait_queue, node* p) {
  if(wait_queue->size() > 0) {
    for(vector<process>::iterator i=wait_queue->begin(); i!=wait_queue->end(); ++i) {
      if(i->timeleft == 0) {
        i->priority += i->io;
        if(i->priority > 99) {
          i->priority = 99;
          if(i->absolute < 50) {
            i->priority = 49;
          }
        } 
        if(i->priority > 49) {
          if(i->absolute < 50) {
            i->priority = 49;
          }
        } 
        insert(p, i->priority, *i);
        i = wait_queue->erase(i);
      }
    } 
  }
}

void demote(process p, node* head) {
  //p->priority-
}

int main() {

  //int scheduled;
  //int wait_time;
  bool start = true;
  int slice = 8;
  int processor_slice=0;
  bool processing = false;
  bool pending_arrival = true;
  int tick=0;
  process being_processed; // a tmp process we use in multiple spots
  vector<process> arrivals;
  vector<process> wait_queue;
  int process_count=0;
  struct node *rr_tree;
  struct node *curr_node;
  struct root *head;

  fstream myfile ("10_processes"); 
  int count=0;
  string buf;
  string line;
  int c=0;

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
        being_processed.arrival=atoi(tokens[2].c_str());
        being_processed.priority=atoi(tokens[3].c_str());
        being_processed.io=atoi(tokens[5].c_str()); 
        being_processed.absolute-atoi(tokens[3].c_str());
        being_processed.trigger_age_up=0;
        ++process_count;
        arrivals.push_back(being_processed);
      }
    }
  } else cout << "Unable to open file"; 
  
  ageing_time = 100;
   
  /*cout << "processes: " << '\n';
  */
  //array to hold our ready queues

  /*process p0 = {1, 8, 0, 23, 8, 0};
  process p1 = {2, 4, 34, 17, 4, 0};
  process p2 = {3, 109, 2, 5, 109, 0};
  process p3 = {4, 200, 15, 3, 200, 0};
  process p4 = {5, 130, 15, 8, 130, 0};
  
  vector<process> arrivals(5);
  arrivals[0] = p0;
  arrivals[1] = p1;
  arrivals[2] = p2;
  arrivals[3] = p3;
  arrivals[4] = p4;
  int process_count = 5;*/
  sort (arrivals.begin(), arrivals.end(), myfunction);
  vector<process>::iterator iter=arrivals.begin();
  vector<process> chunk_to_arrive;
  for(vector<process>::iterator i=arrivals.begin(); i<arrivals.end(); ++i) {
    cout << i->p_id << '\n';
  }
  
  //make 100 nodes with 100 queues
  rr_tree = insert(NULL, 0);
  for(int num_nodes=1; num_nodes<100; ++num_nodes) {
    insert(rr_tree, num_nodes);
  }
 
  cout << "----------" << '\n';
  //while master vector still has processes to be pushed to the ready queue
  while(process_count != 0) {
    //cout << "amount of processes to process left: " << process_count << '\n';
      while(iter->arrival == tick) { 
        if(start) {
          int key = iter->priority;
          rr_tree = insert(NULL, key, *iter);
          head->top = rr_queue;
          start = false;
          cout << "started by pushing: " << rr_tree->rr->p_id << '\n';
          cout << "-----------------" << '\n';
        } else {
          cout << "what were pushin... " << iter->p_id << " with burst of: " << iter->timeleft << " and a priority of: " << iter->priority << '\n';
          int key = iter->priority;
          insert(rr_queue, key, *iter);
          
          cout << "clock tick when we push to the queue: " << tick << '\n';
          cout << "_________________" << '\n';
        }
      //chunk_to_arrive.clear();
      ++iter;
      }
      /*
      //if tree isn't empty start processing
      if(!processing) {
        //if the tree has a size
        if(rr_queue != NULL) {
          curr_node = findMax(rr_queue);
          being_processed = *curr_node->rr;
          remove(rr_queue, being_processed.priority);
          processing = true;
        }
      }

      //
      if(processing) {
        if(processor_slice == slice) {
          // check for clock interrupt
          if(being_processed.burst > 0) {
            demote(being_processed, rr_queue);
          }
        } else if(processor_slice == (slice-1)) {
        // check for io
          if(being_processed.io > 0) {
            being_processed.timeleft = being_processed.io;
            wait_queue.push_back(being_processed);
            processing = false;
          }  
        } else {
          being_processed.burst;
          //continue processing
          if(being_processed.burst <= 0) {
            processing = false;
            --process_count;
          } else {
            being_processed.burst -= 1;
          }
        }
        sleuthIO(&wait_queue, rr_queue);
      }
      /*
    ++tick;
    if(tick == 8000) {
      break;
    }
  }
  cout << "We done foo, total ticks are: " << tick << '\n';
  cout << head->top->key << '\n';
  cout << rr_queue->left->key << " " << rr_queue->right->key << '\n';
  cout  << " " << rr_queue->right->left->key << " " << rr_queue->right->right->key << '\n';
  cout << "       " << " " << rr_queue->right->right->left->key << '\n'; 
}
