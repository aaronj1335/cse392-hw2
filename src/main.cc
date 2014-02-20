#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

#include "scan.h"

using namespace std;

int main() {
  istream_iterator<int> start(cin), end;
  vector<int> numbers(start, end);

  seqScan((void*) &numbers[0], numbers.size(), sizeof numbers[0], add);

  copy(numbers.begin(), numbers.end(), ostream_iterator<int>(cout, "\n"));

  return 0;
}

