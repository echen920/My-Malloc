#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SYSTEM_MALLOC 1

struct node {
    struct node *next;
    struct node *prev;
    int size;
    int freed;
};

// initialize global variable to keep track of the linked list
struct node *head = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* mymalloc: allocates memory on the heap of the requested size. The block
             of memory returned should always be padded so that it begins
             and ends on a word boundary.
     unsigned int size: the number of bytes to allocate.
     retval: a pointer to the block of memory allocated or NULL if the 
             memory could not be allocated. 
             (NOTE: the system also sets errno, but we are not the system, 
                    so you are not required to do so.)
*/
void *mymalloc(unsigned int size) {

    pthread_mutex_lock(&mutex);
    if (head != NULL) {
	// creating a temporary copy of the linked list to do the search
        struct node *tmp;
        tmp = head;
	    // if the current node is free to be used
        while (tmp->next) {
		if (tmp->freed == 0) {
		// indicate that the node is no longer free
		// if the node size is bigger than current metadata + size + new metadata
		// then we can split the node into two parts
                if (tmp->size > (size + sizeof(struct node)*2)) {
                    struct node *new_node;
		    // converting the pointer type to void* to make sure the pointer
		    // arithmetic is correct.
                    new_node = (void *)tmp + size + sizeof(struct node);
                    new_node->freed = 0;
                    new_node->size = tmp->size - size - sizeof(struct node);
                    new_node->next = tmp->next;
		    new_node->prev = tmp;
		    tmp->next->prev = new_node;
                    tmp->size = size;
                    tmp->next = new_node;
		    tmp->freed = 1;
                    pthread_mutex_unlock(&mutex);
		    // return the pointer address without metadata where memory is available to use
                    return (void *)tmp + sizeof(struct node);
                } else if (tmp->size >= size) {
		    tmp->freed = 1;
                    pthread_mutex_unlock(&mutex);
                    return (void *)tmp + sizeof(struct node);
		}
	    	}
            	tmp = tmp->next;
        }

	// checking the last node in the linked list
        if (tmp->freed == 0) {
                if (tmp->size > size + sizeof(struct node)*2) {
                    struct node *new_node;
                    new_node = (void *)tmp + size + sizeof(struct node);
                    new_node->freed = 0;
                    new_node->size = tmp->size - size - sizeof(struct node);
                    new_node->next = tmp->next;
		    new_node->prev = tmp;
		    if (tmp->next) {
		    	tmp->next->prev = new_node;
		    }
                    tmp->size = size;
                    tmp->next = new_node;
                    tmp->freed = 1;
                    pthread_mutex_unlock(&mutex);
                    return (void *)tmp + sizeof(struct node);
                } else if (tmp->size >= size) {
                    tmp->freed = 1;
                    pthread_mutex_unlock(&mutex);
                    return (void *)tmp + sizeof(struct node);
		}
                //pthread_mutex_unlock(&L.lock);
                //return (void *)tmp + sizeof(struct node);
        }
	    // the entire linked list has no available node, then create a new one
            struct node *return_ptr;
            return_ptr = sbrk(sizeof(struct node) + size);
            return_ptr->size = size;
            return_ptr->freed = 1;
            tmp->next = return_ptr;
	    return_ptr->prev = tmp;
            pthread_mutex_unlock(&mutex);
            return (void *)return_ptr + sizeof(struct node);
    } else {
	// head is null, we need to initialize
        struct node *return_ptr;
        return_ptr = sbrk(sizeof(struct node) + size);
        return_ptr->size = size;
        return_ptr->freed = 1;
    	return_ptr->next = NULL;
    	return_ptr->prev = NULL;
        head = return_ptr;
        pthread_mutex_unlock(&mutex);
        return (void *)head + sizeof(struct node);
    }
}

/* myfree: unallocates memory that has been allocated with mymalloc.
     void *ptr: pointer to the first byte of a block of memory allocated by 
                mymalloc.
     retval: 0 if the memory was successfully freed and 1 otherwise.
             (NOTE: the system version of free returns no error.)
*/
unsigned int myfree(void *ptr) {
    pthread_mutex_lock(&mutex);
    // pointer return by mymalloc is the address of available memory space
    // we need to find the metadata
	ptr-=sizeof(struct node);
    if (head) {
        struct node *tmp;
        tmp = head;
        while (tmp->next) {
            // find the corresponding node according to the pointer address and free the space
            if (tmp == ptr) {
                tmp->freed = 0;
                // if the next memory block is free then increase the current node's size and release
                // the next memory block
        		if (tmp->next && tmp->next->freed == 0) {
        			tmp->size += ((tmp->next->size) + sizeof(struct node));
        			if (tmp->next->next) {
        				tmp->next = tmp->next->next;
        				tmp->next->prev = tmp;
        			}
        		}
                // if the previous memory block is free then increase the previous node's size and release
                // the current memory block
        		if (tmp->prev && tmp->prev->freed == 0) {
        			tmp->prev->size += (tmp->size + sizeof(struct node));
        			tmp->prev->next = tmp->next;
        			if (tmp->next) {
        				tmp->next->prev = tmp->prev;
        			}
        		}
                pthread_mutex_unlock(&mutex);
                return 0;
            }
            tmp = tmp->next;
        }
        // check the last node
        if (tmp == ptr) {
            tmp->freed = 0;
            // since it's the last node, we only check if the previous memory block is free
    	    if (tmp->prev) {
        		if (tmp->prev->freed == 0) {
        			tmp->prev->size += (tmp->size + sizeof(struct node));
        			tmp->prev->next = NULL;
        		}
    	    }
            pthread_mutex_unlock(&mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex);
    return 1;
}

