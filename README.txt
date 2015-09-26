README mymemory.c

In this assignment, in order to imitate the memory heap structure, I need to find a way to keep track 
of the size of the memory allocated as well as other information. Therefore I use a node struct to 
represent each memory block allocated by sbrk(). This way, I can keep track of where the memory 
address, the size, the availability to use, the previous and next memory block. By implementing nodes 
with linked list, I'm able to know exactly where the memory block starts and where it ends.
												
The algorithm I selected is the first-fit algorithm. The first fit algorithm finds the first 
available memory block in the linked list and allocate the memory there. If the available memory 
space is larger than its required space, then the memory block is splited into two parts and the 
unused part will be available to use for the next memory allocation.
												
