#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

typedef struct process {
  int p_id;
  int burst;
  int arrival;
  int priority;
  int trigger_age_up;
}process;

int main () {
  string line;
  string buf;
  ifstream myfile ("100k_processes");
  process tmp;
  int count=0;
  int c=0;

  if (myfile.is_open())
    
    while (c < 3 ) {
      //cout << "hello1" << '\n';
      getline (myfile,line);
      stringstream ss(line);
      cout << line << '\n';
      string tokens[12];
      count=0;
      while (ss >> buf) {
        //cout << buf <<'\n';
        tokens[count] = buf;
        cout << tokens[count] << '\n';
        count++;
      }
      tmp.p_id = atoi(tokens[0].c_str());
      tmp.burst = atoi(tokens[0].c_str());      atoi(tokens[0].c_str());
      tmp.priority = atoi(tokens[3].c_str());
      cout << tmp.p_id << '\n' << tmp.burst << '\n' << tmp.arrival << '\n' << tmp.priority << '\n';
      cout << '\n';
      
      c++;
     
    
    
  }
  else cout << "Unable to open file"; 

    myfile.close();
}
