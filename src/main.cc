#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <ctime>

#include "scan.h"

using namespace std;

int main(int argc, char* argv[]) {
  unsigned int dim = 1;
  int opt;
  bool output = true;

  while ((opt = getopt(argc, argv, "nd:")) != -1) {
    switch (opt) {
      case 'n':
        output = false;
        break;
      case 'd':
        dim = atoi(optarg);
        break;
      default: /* '?' */
        cerr << "USAGE: " << argv[0] << " [-d dimensionality]" << endl;
        exit(EXIT_FAILURE);
    }
  }

  istream_iterator<double> start(cin), end;
  vector<double> nums(start, end);

  clock_t startTime = clock();
  parScan((void*) &nums[0], nums.size() / dim, sizeof nums[0] * dim,
      addition(dim));
  clock_t endTime = clock();

  // check if fd #3 is open, if so, write the processor time taken in seconds
  lseek(3, 0, SEEK_CUR);
  if (errno != EBADF) {
    FILE* timeOut = fdopen(3, "w");
    fprintf(timeOut, "%f",
        ((double)endTime - (double)startTime) / (double)CLOCKS_PER_SEC);
  }

  if (output) {
    for (vector<double>::size_type i = 0; i != nums.size(); ++i) {
      cout << nums[i];
      if (i % dim == dim - 1)
        cout << endl;
      else
        cout << ' ';
    }
  }

  return 0;
}

