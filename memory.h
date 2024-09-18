#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>
#include "hole.h"
#include "data.h"

#define INIT_SIZE 2

typedef struct holeList holeList_t;

/* creates & returns an empty hole_list (array), this hole_list will represent
 * a continuous block of memory for task3.3
 * return (hole_list)
 */
holeList_t *holeListCreate();

/* track hole_list size & element, expand the size when needed
 * (code from COMP20003 W3.8 skeleton code)
 */
void holeListEnsureSize(holeList_t *hole_list);

/* check current size of hole_list and expand if needed, then insert hole_node
 * in the order of addr_start (low -> high) and update number of hole_node
 */
void insertHoleSortAddress(holeList_t *hole_list, holeNode_t *release_hole);

/* merge adjacent hole_node which has a connected address into single hole_node
 * to simulate continuous memory block in task3.3
 */
void updateHoleList(holeList_t *hole_list);

/* find best-fitted memory block from hole_list for allocating to process:
 * 1. if size of hole_node == size of memory require by process (perfect fit),
 * the hole_node will be directly allocate (assign) to process_node
 * 2. if size of hole_node > size of memory require by process (approximate fit)
 * the hole_node will perform splitHole() to create perfect fitted hole_node
 * and assigned that to process_node, while the other hole_node still in hole_list
 * return (hole_node): hole_node that assigned to process_node
 */
holeNode_t *assignMemoryProcess(holeList_t *hole_list, processNode_t *process_node);

// free memory which allocate to hole_list
void holeListFree(holeList_t *hole_list);

#endif