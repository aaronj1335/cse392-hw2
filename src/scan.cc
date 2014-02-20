#include <stdio.h>
#include <omp.h>

unsigned int nextPowerOf2(unsigned int n) {
  unsigned int k = 1;
  while (k < n) k *= 2;
  return k;
}

void addition(void* a, void *b) {
  int* intA = (int*) a;
  int* intB = (int*) b;

  *intA += *intB;
}

void seqScan(void *X, unsigned int n, size_t l, void (*op)(void*, void*)) {
  char* c = (char*) X;

  for (unsigned int i = 1; i < n; i++)
    op((void*) (c + i * l), (void*) (c + (i - 1) * l));
}

void parScan(void *X, unsigned int n, size_t l, void (*op)(void*, void*),
    unsigned int stride) {
  char* c = (char*) X;
  unsigned int nextN = nextPowerOf2(n);

  if (stride >= nextN) return;

  #pragma omp parallel for
    for (unsigned int i = stride - 1; i < nextN; i += stride * 2)
      if (i + stride < n)
        op((void*) (c + (i + stride) * l), (void*) (c + i * l));

  parScan(X, n, l, op, stride * 2);

  if (stride > 1)
    #pragma omp parallel for
      for (unsigned int i  = stride - 1; i < nextN - stride; i += stride)
        if (i + stride / 2 < n)
          op((void*) (c + (i + stride / 2) * l), (void*) (c + i * l));
}
