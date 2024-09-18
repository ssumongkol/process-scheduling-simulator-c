#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include "data.h"
#include "queue.h"

struct processQueue {
	processNode_t **node;
	int size;
	int n;
};

/* creates & returns an empty process_queue (array)
 * return (process_queue)
 */
processQueue_t *processQueueCreate() {
	processQueue_t *queue = malloc(sizeof(*queue));
	assert(queue);
	int size = INIT_SIZE;
	queue->size = size;
	queue->node = malloc(size * sizeof(*(queue->node)));
	assert(queue->node);
	queue->n = 0;
	return queue;
}

/* track process_queue size & element, expand the size when needed
 * (code from COMP20003 W3.8 skeleton code)
 */
void processQueueEnsureSize(processQueue_t *queue) {
	if (queue->n == queue->size) {
		queue->size *= 2;
		queue->node = realloc(queue->node, queue->size * sizeof(*(queue->node)));
		assert(queue->node);
	}
}

/* check current size of process_queue and expand if needed, then enqueue
 * process_node into the end of process_queue and update number of process_node
 */
void enQueue(processQueue_t *queue, processNode_t *node) {
    processQueueEnsureSize(queue);
    queue->node[queue->n] = node;
    (queue->n)++;
}

/* check current size of process_queue and expand if needed, then enqueue
 * process_node in the order of service_time (low -> high), arrival_time (high->low),
 * and process_name lexicographically. Also, update number of process_node
 */
void enQueueSortServiceTime(processQueue_t *queue, processNode_t *node) {
    if (queue->n == 0) {
        enQueue(queue, node);
    } else {
        int i = 0, j;
        processQueueEnsureSize(queue);
        // find the optimal place of this new node
        while (i < queue->n && 
        (getServiceTime(node) > getServiceTime(queue->node[i]) ||
        (getServiceTime(node) == getServiceTime(queue->node[i]) &&
        getArrivalTime(node) > getArrivalTime(queue->node[i])) ||
        (getServiceTime(node) == getServiceTime(queue->node[i]) &&
        getArrivalTime(node) == getArrivalTime(queue->node[i]) &&
        strcmp(getName(node), getName(queue->node[i])) > 0))) {
            i++;
        }
        // i = place to add this node, need to move the remaning node to the right
        j = queue->n - 1;
        while (j >= i) {
            queue->node[j + 1] = queue->node[j];
            j--;
        }
        queue->node[i] = node;
        (queue->n)++;
    }
}

/* dequeue process_node at the head of process_queue and update number of process_node
 * return (process_node)
 */
processNode_t *deQueue(processQueue_t *queue) {
    int i;
    if (queue->node[0] != NULL) {
        processNode_t *return_node = queue->node[0];
        for (i = 0; i < queue->n - 1; i++) {
            queue->node[i] = queue->node[i+1];
        }
        queue->node[i] = NULL;
        (queue->n)--;
        return return_node;
    }
    return NULL;
}

/* verify whether process_queue is empty or not
 * return (int): 1 == process_queue is empty, otherwise 0
 */
int processQueueIsEmpty(processQueue_t *queue) {
    if (queue->n <= 0) {
        return 1;
    }
    return 0;
}

/* get process_node at the head of process_queue
 * return (process_node)
 */
processNode_t *getHeadQueue(processQueue_t *queue) {
	return queue->node[0];
}

/* get number of process_node in process_queue
 * return (int)
 */
int getProcNumber(processQueue_t *queue) {
	return queue->n;
}

// free memory which allocate to process_queue
void processQueueFree(processQueue_t *queue) {
    free(queue->node);
    free(queue);
}