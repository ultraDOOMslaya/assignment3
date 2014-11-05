#include <iostream>
using namespace std;
#include <cstdlib>
#include <sys/timeb.h>

int getMilliCount(){
  timeb tb;
  ftime(&tb);
  int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
  return nCount;
}

int getMilliSpan(int nTimeStart){
  int nSpan = getMilliCount() - nTimeStart;
  if(nSpan < 0)
    nSpan += 0x100000 * 1000;
  return nSpan;
}

int main(){
  printf("\n\n");
  printf("****************************************\n");
  printf("** Millisecond Timer\n");
  printf("****************************************");
  

  printf("\n\nStarting timer...");
  int start = getMilliCount();

  // CODE YOU WANT TO TIME
  for(int i = 0; i < 1000000; i++){
    int a = 55/16;
  }

  int milliSecondsElapsed = getMilliSpan(start);

  printf("\n\nElapsed time = %u milliseconds", milliSecondsElapsed);

  printf("\n\n");
  return 1;
}
