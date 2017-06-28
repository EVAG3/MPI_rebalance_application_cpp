/*
 ============================================================================
 Name        : MPI_rebalance_application_cpp.c
 Author      : Yu
 Version     : 1.0
 Copyright   : Copy with reference to Yu
 Description : This application could re-balance the work of each process.
 It simulate how the process communicate with each other.
 ============================================================================
 */

// no standard sleep function for old version c/c++
// change the define to WINDOWS on windows environment
#define LINUX
//#define WINDOWS
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif

#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <string>
#include <iostream>

#include <mpi.h>

#include "task.h"
#include "helper.h"

using namespace std;

/*
 * Sleep function based on the operating environment
 */
void sleepMilliSeconds(int ms) {
#ifdef LINUX
	usleep(ms * 1000);
#endif
#ifdef WINDOWS
	Sleep(ms);
#endif
}

/*
 * In reality this function may be much more complex.
 * Now we just call for sin function and let it sleep for about 25 milliseconds (based on clock time).
 * The sleep simulates the complex calculation.
 */
double computeResult(double input) {
	int ms = 250 * (rand() % 2 + 1);
	sleepMilliSeconds(ms);
	return sin(input);
}

/*
 * Master will assign the work
 */
void master(int rank, int world_size, Task* allTasks, MPI_Datatype MPI_TASK) {
	Task curTask;
	int nAllTasks = 0;
	int nSubTasks[world_size];
	countOriginalAllTasks(world_size, rank, allTasks, &nAllTasks, nSubTasks);
	printOriginalAllTasks(nAllTasks, nSubTasks, world_size);

	int taskIndex = 0;
	for (int i = 1; i < world_size; i++) {
		while (taskIndex < world_size * MAX_TASKS
				&& allTasks[taskIndex].owner == -1) {
			taskIndex++;
		} // there are at least world_size tasks as we generate it, so don't need to check
		printMasterAssignedTask(i, allTasks[taskIndex]);
		MPI_Send(&allTasks[taskIndex], 1, MPI_TASK, i, 0, MPI_COMM_WORLD);

		taskIndex++;
	}
	int nFinishTasks = 0;
	while (nFinishTasks < nAllTasks) {
		MPI_Status status;
		int flag = 0;
		int slaveRank = -1;
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if (flag == 1) {
			slaveRank = status.MPI_SOURCE;
			MPI_Recv(&curTask, 1, MPI_TASK, slaveRank, 1,
			MPI_COMM_WORLD, &status);
			int index = curTask.owner * MAX_TASKS + curTask.index;
			allTasks[index].result = curTask.result;

			nFinishTasks++;
			while (taskIndex < world_size * MAX_TASKS
					&& allTasks[taskIndex].owner == -1) {
				taskIndex++;
			}
			if (taskIndex < world_size * MAX_TASKS) {
				// more task to assign to this slave
				printMasterAssignedTask(slaveRank, allTasks[taskIndex]);
				MPI_Send(&allTasks[taskIndex], 1, MPI_TASK, slaveRank, 0,
				MPI_COMM_WORLD);
				taskIndex++;
			} else {
				// no more task, send empty task to signal
				Task emptyTask;
				MPI_Send(&emptyTask, 1, MPI_TASK, slaveRank, 0,
				MPI_COMM_WORLD);
			}
		} else {
			// sleep for 5 milliseconds to stop the master from calling slaves all the time
			sleepMilliSeconds(5);
		}
	}
	printMasterFinishAllTask();
}
/*
 * Slave will work on the task and send back the result
 */
void slave(int rank, MPI_Datatype MPI_TASK) {
	Task curTask;
	MPI_Status status;
	while (true) {
		MPI_Recv(&curTask, 1, MPI_TASK, MASTER_RANK, 0, MPI_COMM_WORLD,
				&status);
		if (curTask.owner == -1) {
			printSlaveFinishAllTask(rank);
			break;
		} else {
			printSlaveReceivedTask(rank, curTask);
			// calculate the result
			clock_t startTime;
			startTime = clock();
			curTask.result = computeResult(curTask.input);
			printSlaveFinishATask(rank, curTask, startTime);
			MPI_Send(&curTask, 1, MPI_TASK, MASTER_RANK, 1, MPI_COMM_WORLD);
		}
	}
}

/*
 * The main function to execute
 */
int main(int argc, char *argv[]) {
	// init the MPI	and create MPI type
	MPI_Init(&argc, &argv);
	MPI_Datatype MPI_TASK = createMPIDataTypeStruct();

	// get the size of the processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	if (world_size <= 1) {
		cout
				<< "[ERROR] At least 2 process are needed to form master and slave."
				<< endl;
		exit(-1);
	}

	// get the rank
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// generate the original tasks
	Task* selfTasks = generateInitialTasks(rank);
	printOriginalSelfTasks(rank, selfTasks);

	// send tasks to master
	Task* allTasks = NULL;
	if (rank == MASTER_RANK) {
		allTasks = (Task*) malloc(sizeof(Task) * MAX_TASKS * world_size);
	}
	MPI_Gather(selfTasks, MAX_TASKS, MPI_TASK, allTasks, MAX_TASKS, MPI_TASK,
			MASTER_RANK,
			MPI_COMM_WORLD);

	// the master and slaves work on the tasks
	if (rank == MASTER_RANK) {
		master(rank, world_size, allTasks, MPI_TASK);
	} else {
		slave(rank, MPI_TASK);
	}

	// send back the related results and print out
	MPI_Scatter(allTasks, MAX_TASKS, MPI_TASK, selfTasks, MAX_TASKS, MPI_TASK,
			MASTER_RANK,
			MPI_COMM_WORLD);
	printFinalSelfTasks(rank, selfTasks);

	MPI_Finalize();
	return 0;
}

