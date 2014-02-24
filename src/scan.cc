#include <stdio.h>
#include <omp.h>

#include "scan.h"

unsigned int nextPowerOf2(unsigned int n) {
  unsigned int k = 1;
  while (k < n) k *= 2;
  return k;
}

// GROOOOOOOOOSSSSSSS
void addition1Ddouble(void* a, void *b) {
  *((double*) a) += *((double*) b);
}

void addition4Ddouble(void* a, void *b) {
  *(((double*) a) + 0) += *(((double*) b) + 0);
  *(((double*) a) + 1) += *(((double*) b) + 1);
  *(((double*) a) + 2) += *(((double*) b) + 2);
  *(((double*) a) + 3) += *(((double*) b) + 3);
}

t_op addition(unsigned int dimensionality) {
  return dimensionality == 4? addition4Ddouble : addition1Ddouble;
}

void seqScan(void *X, unsigned int n, size_t l, t_op op) {
  char* c = (char*) X;

  for (unsigned int i = 1; i < n; i++)
    op((void*) (c + i * l), (void*) (c + (i - 1) * l));
}

void parScan(void *X, unsigned int n, size_t l, t_op op, unsigned int stride) {
  char* c = (char*) X;
  // is there a more performant way to do this?
  unsigned int nextN = nextPowerOf2(n);

  if (stride >= nextN) return;

  #pragma omp parallel for
    for (int i = stride - 1; (unsigned) i < nextN; i += stride * 2)
      if (i + stride < n)
        op((void*) (c + (i + stride) * l), (void*) (c + i * l));

  parScan(X, n, l, op, stride * 2);

  if (stride > 1)
    #pragma omp parallel for
      for (int i = stride - 1; (unsigned) i < nextN - stride; i += stride)
        if (i + stride / 2 < n)
          op((void*) (c + (i + stride / 2) * l), (void*) (c + i * l));
}
