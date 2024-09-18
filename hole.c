#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hole.h"

struct holeNode {
    int addr_start;
    int addr_end;
    int size;
    holeNode_t *next;
};

/* creates & returns an empty hole_node with start & end memory address
 * return (hole_node)
 */
holeNode_t *holeCreate(int addr_start, int addr_end) {
	holeNode_t *hole = malloc(sizeof(*hole));
	assert(hole);
    hole->addr_start = addr_start;
    hole->addr_end = addr_end;
    hole->size = addr_end - addr_start;
	hole->next = NULL;
	return hole;
}

/* split single hole_node into 2 hole_node based on the input size
 * 1. hole_node which has size equal to addr_size: used to represent
 *    memory allocated to the process
 * 2. hole_node (remaining) which has size = original_size - allocate_size
 * return (hole_node): hole_node which will be used to allocate process
 */
holeNode_t *splitHole(holeNode_t *hole, int addr_size) {
    holeNode_t *process_hole = holeCreate(hole->addr_start, hole->addr_start + addr_size);
    hole->addr_start += addr_size;
    updateHole(hole);
    return process_hole;
}

// update size of hole_node in case of merge or split
void updateHole(holeNode_t *hole) {
    hole->size = hole->addr_end - hole->addr_start;
}

// assign new addr_start to hole_node
void assignStartAddrHole(holeNode_t *hole, int start_addr) {
	hole->addr_start = start_addr;
}

// assign new addr_end of hole
void assignEndAddrHole(holeNode_t *hole, int end_addr) {
	hole->addr_end = end_addr;
}

// connect current hole_node & next hole_node
void assignNextToHole(holeNode_t *current_hole, holeNode_t *next_hole) {
	current_hole->next = next_hole;
}

/* get addr_start of hole_node
 * return (int)
 */
int getStartAddrHole(holeNode_t *hole) {
	return hole->addr_start;
}

/* get addr_end of hole_node
 * return (int)
 */
int getEndAddrHole(holeNode_t *hole) {
	return hole->addr_end;
}

/* get size (addr_start - addr_end) of hole_nod
 * return (int)
 */
int getSizeHole(holeNode_t *hole) {
	return hole->size;
}

/* get next hole_node that connect to current hole_node
 * return (hole_node)
 */
holeNode_t *getNextHole(holeNode_t *hole) {
	return hole->next;
}

// free memory which allocate to hole_node
void holeFree(holeNode_t *hole) {
    free(hole);
	return;
}