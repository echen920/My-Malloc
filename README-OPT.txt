README-OPT mymemory_opt.c

In mymemory_opt.c I have implemented coalescing of free space in myfree, which means when freeing a pointer the function also checks if the neighbouring memory blocks are also
free. If that’s the case, then it merges the two memory blocks together as one, allowing
a bigger size of memory for the next allocation. As for the header, a pointer to store
the address is removed considering it’s redundant and useless. However, it doesn’t show
significant difference after it’s removed. A best-fit algorithm is considered for
phase two but it shows a longer time and similar max heap extend thus my phase two
is still using the first-fit algorithm.