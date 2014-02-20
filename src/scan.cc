#include <stdio.h>
#include <omp.h>

void add(void* a, void *b) {
  int* intA = (int*) a;
  int* intB = (int*) b;

  *intA += *intB;
}

void seqScan(void *X, unsigned int n, size_t l, void (*op)(void*, void*)) {
  char* c = (char*) X;

  for (unsigned int i = 1; i < n; i++)
    op((void*) (c + i * l), (void*) (c + (i - 1) * l));
}
