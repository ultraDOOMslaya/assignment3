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

//#define DEBUG
//#define DEBUG2

//instance variables
typedef struct process {
  int p_id;
  int burst;
  int arrival;
  int priority;
  int deadline;
  int status; //0 means success, 1 means failed to meet deadline
  int finishTime; //starts at -1 to check for errors (or deadline)
  int startingBurst;
}process;


//method for sorting process structs based on arrival time
static bool sortArrival (const process &x, const process &y) { 
	if(x.arrival == y.arrival){
		if(x.priority == y.priority){
			return (x.p_id < y.p_id);
		} else {
			return (x.priority < y.priority);
		}
	}
	return (x.arrival < y.arrival);
}

static bool sortDeadline (const process* x, const process* y) { 
	//return (x->deadline < y->deadline); 
	if(x->deadline == y->deadline){
		if(x->priority == y->priority){
			return (x->p_id < y->p_id);
		} else {
			return (x->priority < y->priority);
		}
	}
	return (x->deadline < y->deadline);



}

int main() {

	vector<process> procs;
	int numprocess = 0; //set to number of them got from file

	//get process information from file
	
	string infile;
	cout << "Enter the name of the input file:";
	cin >> infile;
	fstream myfile(infile);
	

	
	char realTimeType;
	cout << "Soft or hard real-time? (s/h):";
	cin >> realTimeType;
	bool hardTime;
	if(realTimeType == 'h'){
		hardTime = true;
	} else {
		hardTime = false;
	}

	//fstream myfile ("10_processes.txt");
	//fstream myfile ("100k_processes.txt");

	
	int count = 0;
  	string buf;
  	string line;
  	int c = 0;
  	process currentProcess;
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
		    	bool valid = true;
		    	if(atoi(tokens[0].c_str()) < 0) {valid = false;}
		        currentProcess.p_id=atoi(tokens[0].c_str());
		        if(atoi(tokens[1].c_str()) <= 0) {valid = false;}
		        currentProcess.burst=atoi(tokens[1].c_str());
		        currentProcess.startingBurst=atoi(tokens[1].c_str());
		        if(atoi(tokens[2].c_str()) < 0) {valid = false;}
		        currentProcess.arrival=atoi(tokens[2].c_str());
		        if(atoi(tokens[3].c_str()) < 0) {valid = false;}
		        currentProcess.priority=atoi(tokens[3].c_str());
		        if(atoi(tokens[4].c_str()) < 0) {valid = false;}
		        currentProcess.deadline=atoi(tokens[4].c_str());
		        currentProcess.status = 0;
		        currentProcess.finishTime = -1;
		        
		        if(valid == true){
		        	numprocess++;
		        	procs.push_back(currentProcess);
		    	}
		    }
	    }
	} else {
		cout << "Unable to open file"; 
	}
	

	/*
	//test the input procs
	for(int j = 0; j < numprocess; j++){
		cout << "p_id:" << procs.at(j).p_id << endl;
		cout << "burst:" << procs.at(j).burst << endl;
		cout << "arrival:" << procs.at(j).arrival << endl;
		cout << "priority:" << procs.at(j).priority << endl;
		cout << "deadline:" << procs.at(j).deadline << endl;
		cout << "status:" << procs.at(j).status << endl;
		cout << "finishTime:" << procs.at(j).finishTime << endl;
	}
	*/



	//sort processes based on arrival time
	sort (procs.begin(), procs.end(), sortArrival);

	//just to test that it's putting the processes in the vector correctly
	/*
	for(int i = 0; i < procs.size(); i++){
		cout << "pid: " << procs.at(i).p_id << ", arrival: " << procs.at(i).arrival << ", deadline: " << procs.at(i).deadline << ", burst: " << procs.at(i).burst << endl;
	}
	*/

	//more variables needed before ticks start
	int tick = 0; //the tick #
	vector<queue<process*> > priorityQ(100); //the priority queue for scheduling
	bool running = true; //boolean to see if it should keep running the cpu scheduler
	bool addingArrivals;
	int curArrival = 0; //the current process in the procs vector
	int curQueue; //the priority queue currently running processes

	vector<process*> deadlineQueue;


	//start the cpu
	count = 0; //count number of finished processes
	int waitTime = 0;
	int turnaroundTime = 0;
	while(count < numprocess){

		//cout << "At clock tick: " << tick << endl;
		bool aborting = false;

	
		while(true){ //loop to get rid of all processes that fail to meet deadline
			if(deadlineQueue.size() != 0){
				if(deadlineQueue.front()->deadline <= tick){
					#ifdef DEBUG2
						cout << "Process p_id " << deadlineQueue.front()->p_id << " has failed to meet its deadline of " << deadlineQueue.front()->deadline << endl;
					#endif
					deadlineQueue.front()->status = 1; //set the status to aborted
					waitTime = waitTime + (tick - deadlineQueue.front()->startingBurst - deadlineQueue.front()->arrival);
					turnaroundTime = turnaroundTime + (tick - deadlineQueue.front()->arrival);
					deadlineQueue.front()->finishTime = tick;
					
					if(hardTime == true){
						aborting = true;
						break;
					}
					deadlineQueue.erase(deadlineQueue.begin()); //remove it from the queue
					
					count++;
				} else {
					break;
				}
			} else {
				break;
			}

		}

	if(hardTime == true && aborting == true){
		cout << "Need to abord due to being a hard time system" << endl;
		break;
	}

		if(deadlineQueue.size() != 0){	
			deadlineQueue.front()->burst--;
			#ifdef DEBUG2
				cout << "p_id: " << deadlineQueue.front()->p_id << ", tick: " << tick << ", burst left: " << deadlineQueue.front()->burst << endl;
			#endif
			if(deadlineQueue.front()->burst <= 0){ //finished running
				#ifdef DEBUG2
					cout << "p_id: " << deadlineQueue.front()->p_id << " finished at " << tick << endl;
				#endif
				waitTime = waitTime + (tick - deadlineQueue.front()->startingBurst - deadlineQueue.front()->arrival);
				turnaroundTime = turnaroundTime + (tick - deadlineQueue.front()->arrival);
				deadlineQueue.front()->finishTime = tick;
				deadlineQueue.erase(deadlineQueue.begin()); //take it out
				count++;
			}
		}


		//Add arrivals
		addingArrivals = true;
		while(addingArrivals && curArrival < procs.size()){
			
			if(procs.at(curArrival).arrival == tick) { //if a process is arriving
				#ifdef DEBUG2
					cout << "Adding arrival p_id " << procs.at(curArrival).p_id << ", priority " << procs.at(curArrival).priority << ", at tick " << tick << endl;
				#endif
				deadlineQueue.push_back(&procs.at(curArrival));
				

				sort(deadlineQueue.begin(), deadlineQueue.end(), sortDeadline);
				
				/*
				cout << "size: " << deadlineQueue.size() << endl;
				for (int i = 0; i < deadlineQueue.size(); ++i)
				{
					cout << "p_id: " << deadlineQueue.at(i)->p_id << " deadline: " << deadlineQueue.at(i)->deadline << ", arrival: "<< deadlineQueue.at(i)->arrival <<  endl;
				}
				*/
				curArrival++;
			} else {
				addingArrivals = false;
			}
		}
		
		tick++;
	}



	//output INFO
	/*
	for(int j = 0; j < numprocess; j++){
		cout << "p_id:" << procs.at(j).p_id << endl;
		cout << "burst:" << procs.at(j).burst << endl;
		cout << "arrival:" << procs.at(j).arrival << endl;
		cout << "priority:" << procs.at(j).priority << endl;
		cout << "status:" << procs.at(j).status << endl;
		cout << "finishTime:" << procs.at(j).finishTime << endl;
	}
	*/

	#ifdef DEBUG
		int numCompleted = 0;
		for(int j = 0; j < numprocess; j++){
			if(procs.at(j).status == 0 && procs.at(j).finishTime != -1){

				cout << "Process p_id " << procs.at(j).p_id << " finished at " << procs.at(j).finishTime << endl;
				numCompleted++;
			} else {
				//cout << "Process p_id " << procs.at(j).p_id << " hit it's deadline" << endl;
			}

		}
	#endif

	cout << "Number of processes: " << numprocess << endl;
	#ifdef DEBUG
		cout << "Number of processes completed: " << numCompleted << endl;
	#endif
	cout << "Average Wait Time:" << ((float)waitTime/(float)numprocess) << endl;
	cout << "Average Turnaround Time:" << ((float)turnaroundTime/(float)numprocess) << endl;
	

return 0;
}