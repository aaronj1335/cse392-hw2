void addition(void* a, void *b);
void seqScan(void *X, unsigned int n, size_t l, void (*op)(void*, void*));
void parScan(void *X, unsigned int n, size_t l, void (*op)(void*, void*),
    unsigned int stride = 1);
