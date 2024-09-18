#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "data.h"

#define INIT_SIZE 2

typedef struct processQueue processQueue_t;

/* creates & returns an empty process_queue (array)
 * return (process_queue)
 */
processQueue_t *processQueueCreate();

/* track process_queue size & element, expand the size when needed
 * (code from COMP20003 W3.8 skeleton code)
 */
void processQueueEnsureSize(processQueue_t *queue);

/* check current size of process_queue and expand if needed, then enqueue
 * process_node into the end of process_queue and update number of process_node
 */
void enQueue(processQueue_t *queue, processNode_t *node);

/* check current size of process_queue and expand if needed, then enqueue
 * process_node in the order of service_time (low -> high), arrival_time (high->low),
 * and process_name lexicographically. Also, update number of process_node
 */
void enQueueSortServiceTime(processQueue_t *queue, processNode_t *node);

/* dequeue process_node at the head of process_queue and update number of process_node
 * return (process_node)
 */
processNode_t *deQueue(processQueue_t *queue);

/* verify whether process_queue is empty or not
 * return (int): 1 == process_queue is empty, otherwise 0
 */
int processQueueIsEmpty(processQueue_t *queue);

/* get process_node at the head of process_queue
 * return (process_node)
 */
processNode_t *getHeadQueue(processQueue_t *queue);

/* get number of process_node in process_queue
 * return (int)
 */
int getProcNumber(processQueue_t *queue);

// free memory which allocate to process_queue
void processQueueFree(processQueue_t *queue) ;

#endif