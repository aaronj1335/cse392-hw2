typedef void (*t_op)(void*, void*);
t_op addition(unsigned int dimensionality);
void seqScan(void *X, unsigned int n, size_t l, t_op op);
void parScan(void *X, unsigned int n, size_t l, t_op op,
    unsigned int stride = 1);
