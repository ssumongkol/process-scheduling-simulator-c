#ifndef _DATA_H_
#define _DATA_H_

#include <stdio.h>
#include "hole.h"

#define MAX_STRING_LEN 8
#define FLOAT_CONVERTER 1.0

typedef struct processNode processNode_t;

/* creates & returns an empty process_node
 * return (process_node)
 */
processNode_t *nodeCreate();

/* reads a process data from in_file(.txt) and stored in processNode struct
 * return (process_node): filled with process data
 */
processNode_t *dataRead(FILE *in_file); 

// assign PID to process_node for controlling simulated process (task4)
void assignPIDNode(processNode_t *node, int pid);

// assign SHA to process_node which return from simulated process after it terminated
void assignSHANode(processNode_t *node, char *sha);

// assign hole_node to process_node to represent memory allocation
void assignMemNodeToNode(processNode_t *process_node, holeNode_t *mem_node);

/* calculate process_node overhead (turnaround_time/service_time)
 * return (float)
 */
float computeOverHead(processNode_t *node, int time);

/* release hole_node of process_node
 * return (hole_node)
 */
holeNode_t *releaseMemNodeProcess(processNode_t *node);

/* get hole_node (memory allocation) of process_node
 * return (hole_node)
 */
holeNode_t *getMemNode(processNode_t *node);

/* get PID of process_node
 * return (int)
 */
int getPID(processNode_t *node);

/* get pipeParentChild[2] (pipe for Parent process to write & Child process to read data)
 * of process_node
 * return (int pipe[2]): file descriptor
 */
int *getPipeParentChild(processNode_t *node);

/* get pipeChildParent[2] (pipe for Child process to write & Parent process to read data)
 * of process_node
 * return (int pipe[2]): file descriptor
 */
int *getPipeChildParent(processNode_t *node);

/* get arrival_time of process_node
 * return (int)
 */
int getArrivalTime(processNode_t *node);

/* get process_name of process_node
 * return (char *): maximum length is 8
 */
char *getName(processNode_t *node);

/* get service_time of process_node
 * return (int)
 */
int getServiceTime(processNode_t *node);

/* get remaining time that process_node need to execute to complete the its job
 * return (int)
 */
int getServiceRemainingTime(processNode_t *node);

/* get amount of memory that process_node require to execute a task
 * return (int)
 */
int getMemoryRequire(processNode_t *node);

/* update remaining time that process_node need to be execute after process_node
 * is executed by process manager in each cycle
 */
void updateTimeProcess(processNode_t *node, int qvalue);

// prints process data from process_node (for debugging purpose only)
void printProcess(processNode_t *node);

// print detail after successful memory allocation to process_node
void printMemoryAllocateProcess(processNode_t *node, int time);

// print detail when process_node get executed by process manager
void printExecutedProcess(processNode_t *node, int time);

// print detail when process_node is finished
void printFinishedProcess(processNode_t *node, int time, int remain_proc);

// print detail when simulated process is terminated
void printSHAProcess(processNode_t *node, int time);

// free memory which allocate to process_node
void processFree(processNode_t *head_node);

#endif