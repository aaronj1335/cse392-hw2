#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <ctime>

#include <string.h>

#include "scan.h"

using namespace std;

int main(int argc, char* argv[]) {
  unsigned int dim = 1;
  int opt;
  bool output = true;
  int mockSize = 0;
  istream_iterator<double> start, end;
  vector<double> nums;

  while ((opt = getopt(argc, argv, "nd:m:")) != -1) {
    switch (opt) {
      case 'n': // no output
        output = false;
        break;
      case 'd':
        dim = atoi(optarg);
        break;
      case 'm': // mock input, requires the number of megabytes
        mockSize = atoi(optarg);
        break;
      default: /* '?' */
        cerr << "USAGE: " << argv[0]
          << " [-d dimensionality]"
          << " [-n]"
          << endl;
        exit(EXIT_FAILURE);
    }
  }

  if (mockSize) {
    nums = vector<double>(mockSize * dim * (1 << 20));
  } else {
    start = istream_iterator<double>(cin);
    nums = vector<double>(start, end);
  }

  clock_t startTime = clock();
  parScan((void*) &nums[0],
          nums.size() / dim,
          sizeof nums[0] * dim,
          addition(dim));
  clock_t endTime = clock();

  // check if fd #3 is open, if so, write the processor time taken in seconds
  lseek(3, 0, SEEK_CUR);
  if (errno != EBADF && errno != ESPIPE) {
    FILE* timeOut = fdopen(3, "w");
    fprintf(timeOut, "%f",
        ((double) endTime - (double) startTime) / (double) CLOCKS_PER_SEC);
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

