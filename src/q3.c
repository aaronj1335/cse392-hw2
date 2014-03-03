# include <ctype.h>
# include <stdint.h>
# include <time.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <math.h>
# include <omp.h>
# include <mpi.h>

#define COMPARE(a, b) (((a) > (b)) - ((a) < (b)))

static int DEBUG=0;
static int OMP_THREAD_COUNT=8;

int compareInt(const void *a, const void *b)
{
  uint32_t ua = *(uint32_t*) a;
  uint32_t ub = *(uint32_t*) b;
  return COMPARE(ua, ub);
}

uint32_t* allocateIntArray(size_t n, size_t size)
{
  uint32_t *base;
  printf ("Allocating memory for %0d array elements.\n\n", n);
  base = (uint32_t*)malloc(n*size);
  if (base==NULL) {
    fprintf(stderr, "Cannot allocate memory for %0d array elements.\n", n);
    abort();
  }
  return base;
}

void getIntArray(uint32_t *base, size_t n)
{
  uint32_t lfsr = 1u;
  size_t count = 0;
  while (count < n)
    {
      /* taps: 32 31 29 1; feedback polynomial: x^32 + x^31 + x^29 + x + 1 */
      lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xD0000001u);
      *(base+count)=lfsr;
      ++count;
    }
  return;
} 

void getSortedIntArray(uint32_t *base, size_t n)
{
  getIntArray(base, n);
  qsort(base, n, sizeof(uint32_t), compareInt);  
  return;
} 

void displayArray(uint32_t *base, size_t n)
{
  for(size_t i=0; i< n; ++i) printf("INDEX: %d VALUE: %u\n", i, *(base+i));
  printf("\n");
}

void* ompBSearch(const void* key, const void* base, size_t n, size_t size, 
                 int (*cmp)(const void* keyval, const void* datum))
{
  uint32_t *arr = (uint32_t*)base;
  uint32_t *result = NULL;
  if (DEBUG) displayArray(arr, n);
  #pragma omp parallel num_threads(OMP_THREAD_COUNT)
  {
    uint32_t *mid_idx;
    uint32_t *res = NULL;
    uint32_t mid_val;
    int id = omp_get_thread_num();
    int t = omp_get_num_threads();
    size_t block_size=ceil((double)n/t);
    size_t seg_start = id*block_size;
    size_t seg_end = (id+1)*block_size-1;
    int c, cl, cu;
    if (DEBUG) printf("\nIn thread ID %d. Total threads %d.\n", id, t);
    if (DEBUG) printf("(TID: %d of %d) start:%d end:%d bsize: %d baseval: %u\n", id, t-1, seg_start, seg_end, block_size, *arr);
    if (seg_start < n) {
      if (seg_end>=n || (id==t-1 && seg_end<n-1)) {
        seg_end=n-1;
        block_size=seg_end-seg_start+1;
        if (DEBUG) printf("(TID: %d of %d) end:%d bsize: %d\n", id, t-1, seg_end, block_size);
      }
      if (DEBUG) printf("(TID: %d of %d) start_val:%u end_val:%u\n", id, t-1, *(arr+seg_start), *(arr+seg_end));
      if (cmp(&key, (arr+seg_start))>=0 && cmp(&key, (arr+seg_end))<=0) {
        if (DEBUG) printf("(TID: %d of %d) Key found in this block\n", id, t-1);
        mid_idx=arr+seg_start+block_size/2;
        mid_val=*mid_idx;
        c=cmp(&key, &mid_val);
        cl=cmp(&key, (arr+seg_start)) + cmp(&key, &mid_val);
        cu=cmp(&key, &mid_val) + cmp(&key, (arr+seg_end));
        if (DEBUG) printf("(TID: %d of %d) mid_idx: arr+%d mid_val: %u c: %d cl: %d cu: %d\n", id, t-1, seg_start+block_size/2, mid_val, c, cl, cu);
        if (block_size!=1 || c==0) { //same as this (block_size==1 && c!=0)
          if (c==0) {
            if (DEBUG) printf("(TID: %d of %d) Found key\n", id, t-1);
            res=mid_idx;
          }
          else if (cl==0 || cl==-1) {
            if (DEBUG) printf("(TID: %d of %d) Search in lower array of %d elements\n", id, t-1, block_size/2);
            res=ompBSearch(key, (arr+seg_start), block_size/2, size, cmp);
          }
          else if (cu==0 || cu==1) {
            if (DEBUG) printf("(TID: %d of %d) Search in upper array of %d elements\n", id, t-1, (block_size-block_size/2-1));
            res=ompBSearch(key, (mid_idx+1), (block_size-block_size/2-1), size, cmp);
          }
          else {
            fprintf(stderr, "Unknown state in OMP BSearch.\n\n");
            abort();
          }
          if (res != NULL) result = res;
        }
        else {
          if (DEBUG) printf("Reached end of array and match not found.\n");
        }
      }
      else {
        if (DEBUG) printf("(TID: %d of %d) Not in this block\n", id, t-1);
      }
    }
  }
  return result;
}

int main(int argc, char **argv)
{
  uint32_t *arr;
  size_t size=sizeof(uint32_t);
  size_t n;
  uint32_t key;
  size_t idx;
  uint32_t *res;
  // get opts
  int c;
  int flag_sequential=1;
  int flag_omp=0;
  int flag_mpi=0;
  int flag_random_key=0;
  size_t   value_array_size=NULL;
  uint32_t value_key=NULL;

  while ((c = getopt (argc, argv, "n:k:t:")) != -1)
    switch (c)
      {
      case 'n':
        value_array_size = (size_t)atoi(optarg);
        break;
      case 'k':
        value_key = (uint32_t)atoi(optarg);
        break;
      case 't':
        OMP_THREAD_COUNT = (uint32_t)atoi(optarg);
        break;
      case '?':
        if (optopt == 'n' || optopt == 'k')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        abort ();
      }
  if (value_array_size == NULL) {
    value_array_size = 1e2;
    printf("Array size not specified. Using default size %0d.\n\n", value_array_size);
  }
  
  if (value_key==NULL) {
    printf("No key specified.\nSetting random key flag.\n\n");
    flag_random_key=1;
  }
  n=value_array_size;
  arr=allocateIntArray(n, size);
  getSortedIntArray(arr, n);
  if (DEBUG) displayArray(arr, n);
  if (flag_random_key == 1) {
    printf("Computing random key.\n");
    srand(time(NULL));
    idx=rand()%n; // idx between 0 and n-1
    value_key=*(arr+idx);
    printf("Random key is %0u at index %0d.\n\n", value_key, idx);
  }
  key=value_key;
  res=NULL;
  printf("Running BSearch\n\n");
  res = (uint32_t*) ompBSearch(key, arr, n, size, compareInt);
  if (res == NULL)
    printf("Key %0u not found in array.\n\n", key);
  else {
    printf("Key %0u is found in the array.\n\n", *res);
  }
  return 0;
}
