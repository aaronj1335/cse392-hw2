#include <getopt.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

#include "scan.h"

using namespace std;

int main(int argc, char* argv[]) {
  unsigned int dim = 1;
  int opt;

  while ((opt = getopt(argc, argv, "d:")) != -1) {
    switch (opt) {
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

  parScan((void*) &nums[0], nums.size() / dim, sizeof nums[0] * dim,
      addition(dim));

  for (vector<double>::size_type i = 0; i != nums.size(); ++i) {
    cout << nums[i];
    if (i % dim == dim - 1)
      cout << endl;
    else
      cout << ' ';
  }

  return 0;
}

