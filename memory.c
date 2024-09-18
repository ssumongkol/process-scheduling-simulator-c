#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hole.h"
#include "data.h"
#include "memory.h"

struct holeList {
	holeNode_t **hole;
	int size;
	int n;
};

/* creates & returns an empty hole_list (array), this hole_list will represent
 * a continuous block of memory for task3.3
 * return (hole_list)
 */
holeList_t *holeListCreate() {
	holeList_t *hole_list = malloc(sizeof(*hole_list));
	assert(hole_list);
	int size = INIT_SIZE;
	hole_list->size = size;
	hole_list->hole = malloc(size * sizeof(*(hole_list->hole)));
	assert(hole_list->hole);
	hole_list->n = 0;
	return hole_list;
}

/* track hole_list size & element, expand the size when needed
 * (code from COMP20003 W3.8 skeleton code)
 */
void holeListEnsureSize(holeList_t *hole_list) {
	if (hole_list->n == hole_list->size) {
		hole_list->size *= 2;
		hole_list->hole = realloc(hole_list->hole, hole_list->size * sizeof(*(hole_list->hole)));
		assert(hole_list->hole);
	}
}

/* check current size of hole_list and expand if needed, then insert hole_node
 * in the order of addr_start (low -> high) and update number of hole_node
 */
void insertHoleSortAddress(holeList_t *hole_list, holeNode_t *release_hole) {
    holeListEnsureSize(hole_list);
    if (hole_list->n == 0) {
        hole_list->hole[hole_list->n] = release_hole;
    } else {
        int i = 0, j;
        while (i < hole_list->n && getStartAddrHole(release_hole) > getStartAddrHole(hole_list->hole[i])) {
            i++;
        }
        j = hole_list->n - 1;
        while (j >= i) {
            hole_list->hole[j + 1] = hole_list->hole[j];
            j--;
        }
        hole_list->hole[i] = release_hole;
    }
    (hole_list->n)++;
}

/* merge adjacent hole_node which has a connected address into single hole_node
 * to simulate continuous memory block in task3.3
 */
void updateHoleList(holeList_t *hole_list) {
    int i = 0, j;
    while (i < hole_list->n - 1) {
        if (getEndAddrHole(hole_list->hole[i]) == getStartAddrHole(hole_list->hole[i+1])) {
            assignEndAddrHole(hole_list->hole[i], getEndAddrHole(hole_list->hole[i+1]));
            updateHole(hole_list->hole[i]);
            holeFree(hole_list->hole[i+1]);
            j = i + 1;
            while (j <= hole_list->n - 2) {
                hole_list->hole[j] = hole_list->hole[j + 1];
                j++;
            }
            hole_list->hole[j] = NULL;
            hole_list->n--;
        } else {
            i++;
        }
    }
}

/* find best-fitted memory block from hole_list for allocating to process:
 * 1. if size of hole_node == size of memory require by process (perfect fit),
 * the hole_node will be directly allocate (assign) to process_node
 * 2. if size of hole_node > size of memory require by process (approximate fit)
 * the hole_node will perform splitHole() to create perfect fitted hole_node
 * and assigned that to process_node, while the other hole_node still in hole_list
 * return (hole_node): hole_node that assigned to process_node
 */
holeNode_t *assignMemoryProcess(holeList_t *hole_list, processNode_t *process_node) {
    // if there is no memory left, return NULL
    if (hole_list->n == 0) {
        return NULL;
    }

    holeNode_t *best_fit_hole = hole_list->hole[0];
    int index = 0, i, j;
    for (i = 0; i < hole_list->n; i++) {
        if (getMemoryRequire(process_node) <= getSizeHole(hole_list->hole[i]) &&
        getSizeHole(hole_list->hole[i]) < getSizeHole(best_fit_hole)) {
            best_fit_hole = hole_list->hole[i];
            index = i;
        }
    }
    // check no fitted_hole without created any hole
    if (getSizeHole(best_fit_hole) < getMemoryRequire(process_node)) {
        return NULL;
    } else if (getMemoryRequire(process_node) == getSizeHole(best_fit_hole)) {
        for(j = index; j < hole_list->n - 1; j++){
            hole_list->hole[j] = hole_list->hole[j+1];
        }
        hole_list->hole[j] = NULL;
        (hole_list->n)--;
    } else {
        best_fit_hole = splitHole(best_fit_hole, getMemoryRequire(process_node));
    }
    return best_fit_hole;
}

// free memory which allocate to hole_list
void holeListFree(holeList_t *hole_list) {
    for (int i = 0; i < hole_list->n; i++) {
        if (hole_list->hole[i] != NULL) {
            holeFree(hole_list->hole[i]);
        }
    }
    free(hole_list->hole);
    free(hole_list);
}