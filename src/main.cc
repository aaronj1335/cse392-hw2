#include <stdio.h>
#include "scan.h"

int main() {
  void *X = NULL;
  unsigned int n = 0;

  genericScan(X, n, sizeof(int));

  return 0;
}
