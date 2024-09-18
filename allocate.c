#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "hole.h"
#include "data.h"
#include "queue.h"
#include "memory.h"

#define SJF "SJF"
#define RR "RR"
#define SJF_FLAG 1
#define RR_FLAG 2
#define INFINITE "infinite"
#define BEST_FIT "best-fit"
#define INFINITE_FLAG 1
#define BEST_FIT_FLAG 2
#define MIN_QUANTUM 1
#define MAX_QUANTUM 3
#define START_ADDR 0
#define MAX_ADDR 2048
#define PROCESS_FILE "./process"
#define PROCESS_FILE_OPTION "-v"
#define SHA_SIZE 64
#define IMPLEMENTS_REAL_PROCESS

void processArgs(int argc, char *argv[], int *sflag, int *mflag, int *qvalue, 
char **fileName);
processQueue_t *readAllData(FILE *in_file);
void createProcess(processNode_t *node, int time);
void continueProcess(processNode_t *node, int time);
void suspendProcess(processNode_t *node, int time);
void terminateProcess(processNode_t *node, int time);

int main(int argc, char *argv[]) {
	int sflag, mflag, qvalue, counter = 0;
	char* fileName = NULL;

	processArgs(argc, argv, &sflag, &mflag, &qvalue, &fileName);
    FILE *in_file = fopen(fileName, "r");
    assert(in_file);

	processQueue_t *all_process_queue = readAllData(in_file);
	processQueue_t *input_queue = processQueueCreate();
	processQueue_t *ready_queue = processQueueCreate();
	processNode_t *execute_node = NULL, *temp_node;
	holeNode_t *hole_initial = holeCreate(START_ADDR, MAX_ADDR), *temp_hole;
	holeList_t *hole_list = holeListCreate();
	insertHoleSortAddress(hole_list, hole_initial);

	int num_proc = getProcNumber(all_process_queue);
	float max_overhead = 0, total_overhead = 0, average_overhead, node_overhead, total_TA_time = 0;

	// loop to simulate process manager running behavior
	while (processQueueIsEmpty(all_process_queue) == 0 || 
	processQueueIsEmpty(input_queue) == 0 || 
	processQueueIsEmpty(ready_queue) == 0 || execute_node != NULL) {

		// 1.determine whether running process is finished or not
		if (execute_node != NULL && getServiceRemainingTime(execute_node) == 0) {
			// if chosing best-fit as memory allocation, need to release the memory
			if (mflag == BEST_FIT_FLAG) {
				temp_hole = releaseMemNodeProcess(execute_node);
				insertHoleSortAddress(hole_list, temp_hole);
				updateHoleList(hole_list);
			}
			printFinishedProcess(execute_node, counter, getProcNumber(ready_queue) + getProcNumber(input_queue));
			terminateProcess(execute_node, counter);
			printSHAProcess(execute_node, counter);
			total_TA_time += (counter - getArrivalTime(execute_node));
			node_overhead = computeOverHead(execute_node, counter);
			total_overhead += node_overhead;
			if (node_overhead > max_overhead) {
				max_overhead = node_overhead;
			}
			processFree(execute_node);
			execute_node = NULL;
		}
		
		// 2.added process_node to "input_queue" if arrival_time <= simulation_time(counter)
		while (getProcNumber(all_process_queue) != 0 && 
		getArrivalTime(getHeadQueue(all_process_queue)) <= counter) {
			enQueue(input_queue, deQueue(all_process_queue));
		}

		/* 3.added process_node from "input_queue" to "ready_queue", if memory allocation is done
		 * 4.select process by scheduling algorithm to execute (RUNNING state) for each cycle
		 * 	 4.1 Scheduling by Shortest Job First (SJF)
		 *	 4.2 Scheduled by Round Robin (RR)
		 */
		switch (mflag) {
			case INFINITE_FLAG:
				while (getHeadQueue(input_queue) != NULL) {
					switch (sflag) {
						case SJF_FLAG:
							enQueueSortServiceTime(ready_queue, deQueue(input_queue));
							break;
						case RR_FLAG:
							enQueue(ready_queue, deQueue(input_queue));
							break;
					}
				}
				break;
			case BEST_FIT_FLAG:
				for (int i = 0; i < getProcNumber(input_queue); i++) {
					temp_node = deQueue(input_queue);
					assignMemNodeToNode(temp_node, assignMemoryProcess(hole_list, temp_node));
					if (getMemNode(temp_node) != NULL) {
						switch (sflag) {
							case SJF_FLAG:
								enQueueSortServiceTime(ready_queue, temp_node);
								break;
							case RR_FLAG:
								enQueue(ready_queue, temp_node);
								break;
						}
						printMemoryAllocateProcess(temp_node, counter);
					} else {
						enQueue(input_queue, temp_node);
					}
				}
				break;
		}
		if (execute_node == NULL && processQueueIsEmpty(ready_queue) == 0) {
			/* if execute_node is empty, program will select the process_node
			 * based on scheduling algorithm to execute next
			 */
			execute_node = deQueue(ready_queue);
			printExecutedProcess(execute_node, counter);
		} else if (sflag == RR_FLAG && processQueueIsEmpty(ready_queue) == 0 && 
		execute_node != NULL) {
			/* if scheduling algorithm = RR, and execute_node is not empty,
			 * this mean there is a process_node get executed in prior cycle.
			 * Thus, we will suspend that process and added in to ready_queue again
			 * and pick another process to run.
			 */
			suspendProcess(execute_node, counter);
			enQueue(ready_queue, execute_node);
			execute_node = deQueue(ready_queue);
			printExecutedProcess(execute_node, counter);
		}

		/* 5. actual execute the process_node after selected in this cycle
		 * and do the following:
		 *   5.1 create simulated process, if it's the 1st time execute of that process_node
		 *   5.2 continue/resume simulated process
		 */
		if (execute_node != NULL) {
			if (getServiceTime(execute_node) == getServiceRemainingTime(execute_node)) {
				createProcess(execute_node, counter);
			} else {
				continueProcess(execute_node, counter);
			}
			updateTimeProcess(execute_node, qvalue);
		}
		counter += qvalue;
	}

	// calculate simulation stats
	average_overhead = total_overhead / num_proc;
	fprintf(stdout, "Turnaround time %.0f\nTime overhead %.2f %.2f\nMakespan %d\n",
	ceil(total_TA_time / num_proc), max_overhead, average_overhead, counter-qvalue);

	// free allocated memory
	processQueueFree(all_process_queue);
	processQueueFree(input_queue);
	processQueueFree(ready_queue);
	holeListFree(hole_list);
	fclose(in_file);
	return 0;
}

// Verify and extract option value into variables and detect malformed input
void processArgs(int argc, char *argv[], int *sflag, int *mflag, int *qvalue, 
char **fileName) {
	int opt;
	while ((opt = getopt(argc, argv, ":f:s:m:q:")) != -1) {
		switch(opt) {
			case 'f':
				*fileName = optarg;
				break;
			case 's':
				if (strcmp(optarg, SJF) == 0) {
					*sflag = 1;
				} else if (strcmp(optarg, RR) == 0) {
					*sflag = 2;
				}
				break;
			case 'm':
				if (strcmp(optarg, INFINITE) == 0) {
					*mflag = 1;
				} else if (strcmp(optarg, BEST_FIT) == 0) {
					*mflag = 2;
				}
				break;
			case 'q':
				*qvalue = atoi(optarg);
				break;
			case ':':
				printf("Require value for option: %c\n", optopt);
				exit(EXIT_FAILURE);
				break;
			case '?':
				printf("Unknown option: %c\n", optopt);
				exit(EXIT_FAILURE);
				break;
		}
	}
}

/* read process data from input file (one line represent one process) 
 * to each process_node and added into queue
 */
processQueue_t *readAllData(FILE *in_file) {
	processQueue_t *queue = processQueueCreate();
	processNode_t *node;

	while (feof(in_file) == 0) {
        node = dataRead(in_file);
		enQueue(queue, node);
	}
	return queue;
}

/* initialize two pipe as following:
 * 1.pipeParentChild: for parent process to write & child process to read
 * 2.pipeChildParent: for child process to write & parent process to read
 * Then, create child process for executing process.c to create a simulated process,
 * sending 32-bit simulation time in big-endian format to child process,
 * reading result sending back from child process and verify whether its match
 * with the LSB byte of data that parent process sent or not
 */
void createProcess(processNode_t *node, int time) {
    uint32_t time_32bit = htonl(time);
    uint8_t time_lsb = time_32bit >> 24, time_process;
	pid_t childpid;
    char *argv_process[] = {PROCESS_FILE, PROCESS_FILE_OPTION, 
	getName(node), NULL};

	// create pipe1 (for Parent to Write & Child to Read)
    if (pipe(getPipeParentChild(node)) == -1) {
        perror("pipeParentChild");
        exit(EXIT_FAILURE);
    }

    // create pipe2 (for Child to Write & Parent to Read)
    if (pipe(getPipeChildParent(node)) == -1) {
        perror("pipeChildParent");
        exit(EXIT_FAILURE);
    }

    childpid = fork();

    if (childpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (childpid != 0) { 
		/* parent process
		 * close read side of pipe1 & Write side of pipe2 (closed pipe on the Child end process)
		 */ 
        close(getPipeParentChild(node)[0]);
        close(getPipeChildParent(node)[1]);

        // send 32-bit simluation time to child process by pipeParentChild
        write(getPipeParentChild(node)[1], &time_32bit, sizeof(uint32_t));

        // read 1 byte from stdout of child process from pipeChildParent
        read(getPipeChildParent(node)[0], &time_process, sizeof(uint8_t));

        if (time_process != time_lsb) {
            printf("Error mismatch LSBs (Sent: [%02X], Receive: [%02X])\n", 
			time_lsb, time_process);
			exit(EXIT_FAILURE);
        }
    } else {
		/* child process
		 * 1.point read end of pipeParentChild to stdin of child process
		 * 2.point write end of pipeChildParent to stdout of child process
		 */ 
        dup2(getPipeParentChild(node)[0], STDIN_FILENO);
        dup2(getPipeChildParent(node)[1], STDOUT_FILENO);
        execv(argv_process[0], argv_process);
    }
	assignPIDNode(node, childpid);
}

/* send 32-bit simulation time in big-endian format and SIGCONT signal to 
 * child process in order to resume/continue specific child process,
 * reading result sending back from child process and verify whether its match
 * with the LSB byte of data that parent process sent or not
 */
void continueProcess(processNode_t *node, int time) {
    uint32_t time_32bit = htonl(time);
    uint8_t time_lsb = time_32bit >> 24, time_process;
	int pid = getPID(node);
	int *pipe1 = getPipeParentChild(node);
	int *pipe2 = getPipeChildParent(node);

    if (pid != 0) { 
		/* Parent process
		 * send 32-bit simluation time to child process by writng to pipeParentChild
		 */
        write(pipe1[1], &time_32bit, sizeof(uint32_t));
		
		// sent SIGCONT signal to continue / resuming stop child process
		kill(pid, SIGCONT);

        // read 1 byte from stdout of child proces from pipeChildParent
        read(pipe2[0], &time_process, sizeof(uint8_t));

        if (time_process != time_lsb) {
            printf("Error mismatch LSBs (Sent: [%02X], Receive: [%02X])\n", 
			time_lsb, time_process);
			exit(EXIT_FAILURE);
        }
    }
}

/* send 32-bit simulation time in big-endian format and SIGTSTP signal to 
 * child process in order to suspend specific child process and wait for
 * specific child process to get suspended before executing next instruction
 */
void suspendProcess(processNode_t *node, int time) {
    uint32_t time_32bit = htonl(time);
	int pid = getPID(node);
	int *pipe1 = getPipeParentChild(node);
	int wstatus = 0;

    if (pid != 0) {
        /* Parent process
		 * send 32-bit simluation time to child process by writng to pipeParentChild
		 */
        write(pipe1[1], &time_32bit, sizeof(uint32_t));
		
		// sent SIGTSTP to suspend child process
		kill(pid, SIGTSTP);

		// wait for child process to enter "stop" state
		do {
			pid_t w = waitpid(pid, &wstatus, WUNTRACED);

			if (w == -1) {
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
		} while (!WIFSTOPPED(wstatus));
    }
}

/* send 32-bit simulation time in big-endian format and SIGTERM signal to 
 * child process in order to terminate specific child process,
 * reading 64 byte SHA sending back from child process stored it within process_node
 */
void terminateProcess(processNode_t *node, int time) {
    uint32_t time_32bit = htonl(time);
	int pid = getPID(node);
	int *pipe1 = getPipeParentChild(node);
	int *pipe2 = getPipeChildParent(node);
	char *sha = calloc(SHA_SIZE+1, sizeof(char));
	assert(sha);

    if (pid != 0) {
        /* Parent process
		 * send 32-bit simluation time to child process by writng to pipeParentChild
		 */
        write(pipe1[1], &time_32bit, sizeof(uint32_t));
		
		// sent SIGTERM to terminate child process
		kill(pid, SIGTERM);

        // read 64 byte from stdout of child proces from pipeChildParent
        read(pipe2[0], sha, SHA_SIZE);
    }
	assignSHANode(node, sha);
	free(sha);
}