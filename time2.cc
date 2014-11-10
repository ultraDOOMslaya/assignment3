#include <iostream>
#include <ctime>
using namespace std;

int main()
{
  unsigned long long Int64 = 0;
  clock_t Start = clock();
  for(unsigned short i = 0; i < 1024; ++i)
  {
    for(unsigned short j = i; j > 0; --j)
    {
      Int64 += j + i;
    }
  }
  cout << "Time Difference: " << clock() - Start << endl;
}
