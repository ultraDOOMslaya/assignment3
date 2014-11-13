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

typedef struct process {
  int p_id;
  int burst;
  int startingBurst;
  int arrival;
  int priority;
  int finishTime;
  int ageUpTime; //time remaining before it ages up
  int currentQueue; //current queue the proc is in
}process;


typedef struct rr_queue {
  queue<process*> rr;
  int quantum;
}rr_queue;

//method for sorting process structs based on arrival time
static bool sortArrival (const process &x, const process &y) { return (x.arrival < y.arrival); }

void ageing(vector<rr_queue> *rrQueues, queue<process*> *fcfsQueue, int ticksSinceLast, int numberQueues, int currentQueue, int ageTime){
	//loop through all the queues under the current one and decrement their ageUpTime by ticksSinceLast

	int i;

	if(currentQueue == 0){ //if the first queue is running, only age the 3rd queue and on
		i = 2; //because you dont want to promote the the first queue
	} else {
		i = currentQueue + 1; //else if you are in queue 2 or on, age all the processes in the queues below
	}
	while(i < numberQueues-1){ //do all the queues but the fcfs
		queue<process*> temp;
		while(!rrQueues->at(i).rr.empty()){ //while the current queue isnt empty
			rrQueues->at(i).rr.front()->ageUpTime = rrQueues->at(i).rr.front()->ageUpTime - ticksSinceLast; //subtract from the age time
			
			if(rrQueues->at(i).rr.front()->ageUpTime <= 0){//if their ageUpTime <= 0, promote them up a queue
				//cout << "Promoting queue p_id " << rrQueues->at(i).rr.front()->p_id << " to queue " << i-1 << endl;
				rrQueues->at(i).rr.front()->ageUpTime = ageTime; //reset their age counter
				rrQueues->at(i-1).rr.push(rrQueues->at(i).rr.front());
				rrQueues->at(i).rr.pop();

			} else { //else put them in the temp queue
				//cout << "rr q, p_id " << rrQueues->at(i).rr.front()->p_id << " ageUpTime remaining " << rrQueues->at(i).rr.front()->ageUpTime << endl;
				temp.push(rrQueues->at(i).rr.front());
				rrQueues->at(i).rr.pop();
			}
		}
		rrQueues->at(i).rr = temp; //set rrQueue (now empty) to the temp queue (everything it had minus the possible promotions)
		
		i++;
	}
	//age the fcfs
	queue<process*> temp;
	while(!fcfsQueue->empty()){
		fcfsQueue->front()->ageUpTime = fcfsQueue->front()->ageUpTime - ticksSinceLast;
		if(fcfsQueue->front()->ageUpTime <= 0){
			//cout << "Promoting queue p_id " << fcfsQueue->front()->p_id << " to queue " << numberQueues - 2 << endl;
			fcfsQueue->front()->ageUpTime = ageTime; //reset their age counter
			rrQueues->at(numberQueues-2).rr.push(fcfsQueue->front());
			fcfsQueue->pop();
		} else {
			//cout << "fcfs q, p_id " << fcfsQueue->front()->p_id << " ageUpTime remaining " <<  fcfsQueue->front()->ageUpTime << endl;
			temp.push(fcfsQueue->front());
			fcfsQueue->pop();
		}
	}
	
	*fcfsQueue = temp; //set fcfsQueue (now empty) to the temp queue (holding everything it had minus the promotions)
}



int main() {


	vector<process> procs;
	int numprocess = 0; //set to number of them got from file

	/*
	//get file from user
	string infile;
	cout << "Enter the name of the input file:";
	cin >> infile;
	fstream myfile(infile);
	*/
	//fstream myfile ("20_processes.txt");
	//fstream myfile ("300_processes.txt");
	fstream myfile ("100k_processes.txt");


	/*
	//get number of queues from user (3-5)
	int numberQueues;
	cout << "Enter the number of queues (3-5):";
	cin >> numberQueues;
	*/
	int numberQueues = 3;

	/*
	//get the time slice
	int timeSlice;
	cout << "Enter the time quantum of the first queue: ";
	cin >> timeSlice;
	*/
	int timeSlice = 2; //time quantum of first queue, get this from user eventually too

	/*
	//get the age time
	int ageTime;
	cout << "Enter the time before a process gets promoted (ageing): ";
	cin >> ageTime;
	*/	
	int ageTime = 40; //time to promote a process, promt this from user eventually


	//get process information from file
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
		        if(atoi(tokens[1].c_str()) <= 0) {valid = false;} //burst <= 0 skip it
		        currentProcess.burst=atoi(tokens[1].c_str());
		        currentProcess.startingBurst=atoi(tokens[1].c_str());
		        if(atoi(tokens[2].c_str()) < 0) {valid = false;}
		        currentProcess.arrival=atoi(tokens[2].c_str());
		        if(atoi(tokens[3].c_str()) < 0) {valid = false;}
		        currentProcess.priority=atoi(tokens[3].c_str());
		        currentProcess.ageUpTime = ageTime;
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
	// {pid, burst, arrival, priority, finish time}
	process p0 = {0, 8, 0, 23 -1}; //test data
 	process p1 = {1, 4, 34, 17, -1};
	process p2 = {2, 109, 10, -1};
	process p3 = {3, 200, 15, 3, -1};
	process p4 = {4, 50, 15, 8, -1};

	procs.push_back(p0);
	procs.push_back(p1);
	procs.push_back(p2);
	procs.push_back(p3);
	procs.push_back(p4);
	*/

	//sort processes based on arrival time
	sort (procs.begin(), procs.end(), sortArrival);

	//just to test that it's putting the processes in the vector correctly
	/*
	for(int i = 0; i < procs.size(); i++){
		cout << "pid: " << procs.at(i).p_id << ", arrival: " << procs.at(i).arrival << " and burst time: " << procs.at(i).burst << endl;
	}
	*/


	//set information needed before running
	int curArrival = 0; //the current process in the procs vector
	int tick=0;
	//create queues
	vector<rr_queue> rrQueues(numberQueues-1);
	queue<process*> fcfsQueue;


	//set the time quantums
	for(int i = 0; i < numberQueues - 1; i++){
		rrQueues.at(i).quantum = timeSlice * (i+1);
	}

	/*
	for(int i = 0; i < numberQueues - 1; i++){
		cout << "Time quantum of queue " << i << ": " << rrQueues.at(i).quantum << endl;
	}
	*/
	
	//find the first arrival and start running at that tick
	/*
	while(true){
		if(procs.at(curArrival).arrival == tick){
			rrQueues.at(0).rr.push(&procs.at(curArrival));
			curArrival++;
			break;
		} else {
			tick++;
		}
	}
	cout << "first arrival pid: " << rrQueues.at(0).rr.front()->p_id << " at tick: " << tick <<  endl;
	*/

	//start the cpu
	count = 0; //count number of finished processes
	int quantumRemaining = 0; //dont change processes if the quantum is still running
	int runningQueue = -1; //the queue that the process is running in if the quantum isnt expired
	int ticksSinceLast = 0; //count the number of ticks to age the processes
	int waitTime = 0;
	int turnaroundTime = 0;

	while(count < numprocess){

		//cout << "At clock tick: " << tick << endl;


		//if there isn't something running in RR
		//check the highest priority queue for something to run
		if(quantumRemaining == 0){
			if(!rrQueues.at(0).rr.empty()){ //if theres not a proc running and queue isnt empty
				quantumRemaining = rrQueues.at(0).quantum - 1; //set the quantum
				runningQueue = 0;
				rrQueues.at(0).rr.front()->burst--;
				rrQueues.at(0).rr.front()->currentQueue = 0;
				
				//cout << "Process p_id: " << rrQueues.at(runningQueue).rr.front()->p_id << ", tick: " << tick << ", queue: " << runningQueue << ", remaining burst: " << rrQueues.at(runningQueue).rr.front()->burst << endl;
				if(rrQueues.at(0).rr.front()->burst == 0){ //completed process pop it out of queue
					waitTime = waitTime + (tick - rrQueues.at(0).rr.front()->startingBurst - rrQueues.at(0).rr.front()->arrival);
					turnaroundTime = turnaroundTime + (tick - rrQueues.at(0).rr.front()->arrival);
					rrQueues.at(0).rr.front()->finishTime = tick;
					rrQueues.at(0).rr.pop();
					quantumRemaining = 0;
					count++;
				}


			} else if(!rrQueues.at(1).rr.empty()){
				quantumRemaining = rrQueues.at(1).quantum - 1; //set the quantum
				runningQueue = 1;
				rrQueues.at(1).rr.front()->burst--;
				rrQueues.at(1).rr.front()->currentQueue = 1;
				
				//cout << "Process p_id: " << rrQueues.at(runningQueue).rr.front()->p_id << ", tick: " << tick << ", queue: " << runningQueue << ", remaining burst: " << rrQueues.at(runningQueue).rr.front()->burst << endl;
				if(rrQueues.at(1).rr.front()->burst == 0){ //completed process pop it out of queue
					waitTime = waitTime + (tick - rrQueues.at(1).rr.front()->startingBurst - rrQueues.at(1).rr.front()->arrival);
					turnaroundTime = turnaroundTime + (tick - rrQueues.at(1).rr.front()->arrival);
					rrQueues.at(1).rr.front()->finishTime = tick;
					rrQueues.at(1).rr.pop();
					quantumRemaining = 0;
					count++;

				}

			} else {

				// 3 queues ------------------------------------
				if(numberQueues == 3) {
					if(!fcfsQueue.empty()){
						quantumRemaining = 0; //doesnt keep running fcfs if there is a new arrival
						runningQueue = 2;
						fcfsQueue.front()->burst--;
						fcfsQueue.front()->currentQueue = 2;
						//cout << "Process p_id: " << fcfsQueue.front()->p_id << ", tick: " << tick << ", fcfs queue, remaining burst: " << fcfsQueue.front()->burst << endl;
						if(fcfsQueue.front()->burst <= 0){ //completed process pop it out of queue
							waitTime = waitTime + (tick - fcfsQueue.front()->startingBurst - fcfsQueue.front()->arrival);
							turnaroundTime = turnaroundTime + (tick - fcfsQueue.front()->arrival);
							fcfsQueue.front()->finishTime = tick;
							fcfsQueue.pop();
							count++;

						}
					}

				// 4 queues ------------------------------------
				} else if(numberQueues == 4){
					if(!rrQueues.at(2).rr.empty()){
						quantumRemaining = rrQueues.at(2).quantum - 1;
						runningQueue = 2;
						rrQueues.at(2).rr.front()->burst--;
						rrQueues.at(2).rr.front()->currentQueue = 2;
						//cout << "Process p_id: " << rrQueues.at(runningQueue).rr.front()->p_id << ", tick: " << tick << ", queue: " << runningQueue << ", remaining burst: " << rrQueues.at(runningQueue).rr.front()->burst << endl;
						if(rrQueues.at(2).rr.front()->burst == 0){ //completed process pop it out of queue
							waitTime = waitTime + (tick - rrQueues.at(2).rr.front()->startingBurst - rrQueues.at(2).rr.front()->arrival);
							turnaroundTime = turnaroundTime + (tick - rrQueues.at(2).rr.front()->arrival);
							rrQueues.at(2).rr.front()->finishTime = tick;
							rrQueues.at(2).rr.pop();
							quantumRemaining = 0;
							count++;

						}

					} else if(!fcfsQueue.empty()){
						quantumRemaining = 0; //doesnt keep running fcfs if there is a new arrival
						runningQueue = 3;
						fcfsQueue.front()->burst--;
						fcfsQueue.front()->currentQueue = 3;
						//cout << "Process p_id: " << fcfsQueue.front()->p_id << ", tick: " << tick << ", fcfs queue, remaining burst: " << fcfsQueue.front()->burst << endl;
						if(fcfsQueue.front()->burst <= 0){ //completed process pop it out of queue
							waitTime = waitTime + (tick - fcfsQueue.front()->startingBurst - fcfsQueue.front()->arrival);
							turnaroundTime = turnaroundTime + (tick - fcfsQueue.front()->arrival);
							fcfsQueue.front()->finishTime = tick;
							fcfsQueue.pop();
							count++;

						}
					}

				// 5 queues ------------------------------------
				} else if(numberQueues == 5){ //5 queues
					if(!rrQueues.at(2).rr.empty()){
						quantumRemaining = rrQueues.at(2).quantum - 1;
						runningQueue = 2;
						rrQueues.at(2).rr.front()->burst--;
						rrQueues.at(2).rr.front()->currentQueue = 2;
						//cout << "Process p_id: " << rrQueues.at(runningQueue).rr.front()->p_id << ", tick: " << tick << ", queue: " << runningQueue << ", remaining burst: " << rrQueues.at(runningQueue).rr.front()->burst << endl;
						if(rrQueues.at(2).rr.front()->burst == 0){ //completed process pop it out of queue
							waitTime = waitTime + (tick - rrQueues.at(2).rr.front()->startingBurst - rrQueues.at(2).rr.front()->arrival);
							turnaroundTime = turnaroundTime + (tick - rrQueues.at(2).rr.front()->arrival);
							rrQueues.at(2).rr.front()->finishTime = tick;
							rrQueues.at(2).rr.pop();
							quantumRemaining = 0;
							count++;

						}


					} else if(!rrQueues.at(3).rr.empty()){
						quantumRemaining = rrQueues.at(3).quantum - 1;
						runningQueue = 3;
						rrQueues.at(3).rr.front()->burst--;
						rrQueues.at(3).rr.front()->currentQueue = 3;
						//cout << "Process p_id: " << rrQueues.at(runningQueue).rr.front()->p_id << ", tick: " << tick << ", queue: " << runningQueue << ", remaining burst: " << rrQueues.at(runningQueue).rr.front()->burst << endl;
						if(rrQueues.at(3).rr.front()->burst == 0){ //completed process pop it out of queue
							waitTime = waitTime + (tick - rrQueues.at(3).rr.front()->startingBurst - rrQueues.at(3).rr.front()->arrival);
							turnaroundTime = turnaroundTime + (tick - rrQueues.at(3).rr.front()->arrival);
							rrQueues.at(3).rr.front()->finishTime = tick;
							rrQueues.at(3).rr.pop();
							quantumRemaining = 0;
							count++;

						}


					} else if(!fcfsQueue.empty()){
						quantumRemaining = 0; //doesnt keep running fcfs if there is a new arrival
						runningQueue = 4;
						fcfsQueue.front()->burst--;
						fcfsQueue.front()->currentQueue = 4;
						//cout << "Process p_id: " << fcfsQueue.front()->p_id << ", tick: " << tick << ", fcfs queue, remaining burst: " << fcfsQueue.front()->burst << endl;
						if(fcfsQueue.front()->burst <= 0){ //completed process pop it out of queue
							waitTime = waitTime + (tick - fcfsQueue.front()->startingBurst - fcfsQueue.front()->arrival);
							turnaroundTime = turnaroundTime + (tick - fcfsQueue.front()->arrival);
							fcfsQueue.front()->finishTime = tick;
							fcfsQueue.pop();
							count++;

						}
		
					}	
				} else { //should never get here
					cout << "ERROR, invalid number of queues" << endl;
				}
			} 

		//if something is running in RR with a time quantum still
		} else { //quantum != 0
			quantumRemaining--;
			rrQueues.at(runningQueue).rr.front()->burst--;
			if(tick % 100 == 0)
			cout << "Process p_id: " << rrQueues.at(runningQueue).rr.front()->p_id << ", tick: " << tick << ", queue: " << runningQueue << ", remaining burst: " << rrQueues.at(runningQueue).rr.front()->burst << endl;
			if(rrQueues.at(runningQueue).rr.front()->burst <= 0){
				waitTime = waitTime + (tick - rrQueues.at(runningQueue).rr.front()->startingBurst - rrQueues.at(runningQueue).rr.front()->arrival);
				turnaroundTime = turnaroundTime + (tick - rrQueues.at(runningQueue).rr.front()->arrival);
				if(runningQueue != numberQueues){
					ageing(&rrQueues, &fcfsQueue, ticksSinceLast, numberQueues, runningQueue, ageTime);
				}
				ticksSinceLast = 0;

				rrQueues.at(runningQueue).rr.front()->finishTime = tick;
				rrQueues.at(runningQueue).rr.pop();
				quantumRemaining = 0;
				count++;

			} else if(quantumRemaining == 0){ //if process isnt done and quantum is up, it gets demoted
				if(runningQueue != numberQueues){
					ageing(&rrQueues, &fcfsQueue, ticksSinceLast, numberQueues, runningQueue, ageTime);
				}
				ticksSinceLast = 0;
				//cout << "Demoting p_id " << rrQueues.at(runningQueue).rr.front()->p_id << " to queue " << runningQueue + 1 << endl;
				rrQueues.at(runningQueue).rr.front()->ageUpTime = ageTime; //reset the aging because it's just finished running
				if(runningQueue == numberQueues - 2){ //needs to be put in fcfs (2 becaues queues start at 0)
					fcfsQueue.push(rrQueues.at(runningQueue).rr.front());
					rrQueues.at(runningQueue).rr.pop();

				} else {
					rrQueues.at(runningQueue+1).rr.push(rrQueues.at(runningQueue).rr.front());
					rrQueues.at(runningQueue).rr.pop(); //take out off the current queue then
				}
			}
		}


		//check for new Arrivals
		bool addingArrivals = true;
		while(addingArrivals && curArrival < procs.size()){

			if(procs.at(curArrival).arrival == tick) { //if a process is arriving
				cout << "Adding arrival p_id " << procs.at(curArrival).p_id << ", at tick " << tick << endl;
				rrQueues.at(0).rr.push(&procs.at(curArrival)); //add the process to the first queue
				curArrival++;
			} else {
				addingArrivals = false;
			}
		}

		ticksSinceLast++;
		tick++;
	}

	for(int j = 0; j < numprocess; j++){
		//cout << "Process p_id " << procs.at(j).p_id << " finished at " << procs.at(j).finishTime << endl;
	}

	cout << "Number Processes:" << numprocess << endl;
	cout << "Average Wait Time:" << (waitTime/numprocess) << endl;
	cout << "Average Turnaround Time:" << (turnaroundTime/numprocess) << endl;

	return 0;
}
