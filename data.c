#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hole.h"
#include "data.h"

struct processNode {
	int arrival_time;
	char *name;
	int service_time;
	int service_remaining_time;
	int memory_require;
	int pid;
	int pipeParentChild[2];
	int pipeChildParent[2];
	char *sha;
	holeNode_t *mem_node;
};

/* creates & returns an empty process_node
 * return (process_node)
 */
processNode_t *nodeCreate() {
	processNode_t *node = malloc(sizeof(*node));
	assert(node);
	node->mem_node = NULL;
	return node;
}

/* reads a process data from in_file(.txt) and stored in processNode struct
 * return (process_node): filled with process data
 */
processNode_t *dataRead(FILE *in_file) {
	char *name = (char *) malloc((MAX_STRING_LEN + 1) * sizeof(char));
	assert(name);
	processNode_t *new_node = nodeCreate();
	fscanf(in_file, "%d %s %d %d\n", &new_node->arrival_time, name,
		&new_node->service_time, &new_node->memory_require);
	new_node->name = strdup(name);
	new_node->service_remaining_time = new_node->service_time;
	free(name);
	return new_node;
}

// assign PID to process_node for controlling simulated process (task4)
void assignPIDNode(processNode_t *node, int pid) {
	node->pid = pid;
}

// assign SHA to process_node which return from simulated process after it terminated 
void assignSHANode(processNode_t *node, char *sha) {
	node->sha = strdup(sha);
}

// assign hole_node to process_node to represent memory allocation
void assignMemNodeToNode(processNode_t *process_node, holeNode_t *mem_node) {
	process_node->mem_node = mem_node;
}

/* calculate process_node overhead (turnaround_time/service_time)
 * return (float)
 */
float computeOverHead(processNode_t *node, int time) {
	return ((time - node->arrival_time) * FLOAT_CONVERTER) / node->service_time;
}

/* release hole_node of process_node
 * return (hole_node)
 */
holeNode_t *releaseMemNodeProcess(processNode_t *node) {
	holeNode_t *release_hole = node->mem_node;
	node->mem_node = NULL;
	return release_hole;
}

/* get hole_node (memory allocation) of process_node
 * return (hole_node)
 */
holeNode_t *getMemNode(processNode_t *node) {
	return node->mem_node;
}

/* get PID of process_node
 * return (int)
 */
int getPID(processNode_t *node) {
	return node->pid;
}

/* get pipeParentChild[2] (pipe for Parent process to write & Child process to read data)
 * of process_node
 * return (int pipe[2]): file descriptor
 */
int *getPipeParentChild(processNode_t *node) {
	return node->pipeParentChild;
}

/* get pipeChildParent[2] (pipe for Child process to write & Parent process to read data)
 * of process_node
 * return (int pipe[2]): file descriptor
 */
int *getPipeChildParent(processNode_t *node) {
	return node->pipeChildParent;
}

/* get arrival_time of process_node
 * return (int)
 */
int getArrivalTime(processNode_t *node) {
	return node->arrival_time;
}

/* get process_name of process_node
 * return (char *): maximum length is 8
 */
char *getName(processNode_t *node) {
	return node->name;
}

/* get service_time of process_node
 * return (int)
 */
int getServiceTime(processNode_t *node) {
	return node->service_time;
}

/* get remaining time that process_node need to execute to complete the its job
 * return (int)
 */
int getServiceRemainingTime(processNode_t *node) {
	return node->service_remaining_time;
}

/* get amount of memory that process_node require to execute a task
 * return (int)
 */
int getMemoryRequire(processNode_t *node) {
	return node->memory_require;
}

/* update remaining time that process_node need to be execute after process_node
 * is executed by process manager in each cycle
 */
void updateTimeProcess(processNode_t *node, int qvalue) {
	node->service_remaining_time -= qvalue;
	if (node->service_remaining_time < 0) {
		node->service_remaining_time = 0;
	}
}

// prints process data from process_node (for debugging purpose only)
void printProcess(processNode_t *node) {
	fprintf(stdout, "%d,process_name=%s,%d,%d\n", node->arrival_time, 
            node->name, node->service_time, node->memory_require);
}

// print detail after successful memory allocation to process_node
void printMemoryAllocateProcess(processNode_t *node, int time) {
    fprintf(stdout, "%d,READY,process_name=%s,assigned_at=%d\n", time, 
            node->name, getStartAddrHole(node->mem_node));
}

// print detail when process_node get executed by process manager
void printExecutedProcess(processNode_t *node, int time) {
	fprintf(stdout, "%d,RUNNING,process_name=%s,remaining_time=%d\n", time, 
            node->name, node->service_remaining_time);
}

// print detail when process_node is finished
void printFinishedProcess(processNode_t *node, int time, int remain_proc) {
	fprintf(stdout, "%d,FINISHED,process_name=%s,proc_remaining=%d\n", time, 
            node->name, remain_proc);
}

// print detail when simulated process is terminated
void printSHAProcess(processNode_t *node, int time) {
	fprintf(stdout, "%d,FINISHED-PROCESS,process_name=%s,sha=%s\n", time, 
            node->name, node->sha);
}

// free memory which allocate to process_node
void processFree(processNode_t *node) {
	free(node->name);
	free(node->sha);
	free(node);
}