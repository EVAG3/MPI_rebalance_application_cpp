/*
 * helper.h
 *
 *  Created on: Jun 27, 2017
 *      Author: yu
 */

#ifndef HELPER_H_
#define HELPER_H_

using namespace std;

#include <cstdlib>
#include <stddef.h>
#include <math.h>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>

#include <mpi.h>

#include "task.h"

/* max number of the initial tasks*/
const int MAX_TASKS = 10;

/* the master is the process with rank = 0*/
const int MASTER_RANK = 0;

/*
 * Create the MPI data type for sturct Task
 */
MPI_Datatype createMPIDataTypeStruct() {
	int nItemInTask = 4;
	int blocksLength[nItemInTask] = { 1, 1, 1, 1 };
	MPI_Datatype types[nItemInTask] =
			{ MPI_INT, MPI_INT, MPI_DOUBLE, MPI_DOUBLE };
	MPI_Aint offsets[nItemInTask] = { (int) offsetof(Task, owner),
			(int) offsetof(Task, index), (int) offsetof(Task, input),
			(int) offsetof(Task, result) };
	MPI_Datatype MPI_TASK;
	MPI_Type_create_struct(nItemInTask, blocksLength, offsets, types,
			&MPI_TASK);
	MPI_Type_commit(&MPI_TASK);
	return MPI_TASK;
}

/*
 * Generate the original tasks
 */
Task* generateInitialTasks(int rank) {
	srand((int) time(0) + rank);
	int nTasks = rand() % MAX_TASKS + 1;
	Task* selfTasks = new Task[MAX_TASKS];
	for (int i = 0; i < nTasks; i++) {
		Task task;
		task.owner = rank;
		task.index = i;
		task.input = (double) rand();
		task.result = 0;
		selfTasks[i] = task;
	}
	return selfTasks;
}

/*
 * Print the original tasks of each process
 */
void printOriginalSelfTasks(int rank, Task* selfTasks) {
	int nTasks = 0;
	for (int i = 0; i < MAX_TASKS; i++) {
		if (selfTasks[i].owner == -1) {
			break;
		}
		nTasks++;
	}
	stringstream output;
	output << "[Process] Process (rank = " << rank << ") generate " << nTasks
			<< " initial tasks. Original input: ";

	for (int i = 0; i < MAX_TASKS; i++) {
		if (selfTasks[i].owner == -1) {
			break;
		}
		output << "x = " << selfTasks[i].input << ", ";
	}
	output << endl;
	cout << output.str();
}

/*
 * Count the numbers of tasks received of the task list
 */
void countOriginalAllTasks(int size, int rank, Task* allTasks, int *nAllTasks,
		int* nSubTasks) {
	int subSum = 0;
	int index = 0;
	int i = 0;
	while (i < size * MAX_TASKS) {
		if (allTasks[i].owner == -1) {
			if (subSum != 0) {
				nSubTasks[index] = subSum;
				subSum = 0;
				index++;
			}
			i++;
		} else {
			i++;
			subSum++;
			*nAllTasks = *nAllTasks + 1;
		}
	}
}

/*
 * Print out the original task list of all
 */
void printOriginalAllTasks(int nAllTasks, int* nSubTasks, int size) {
	stringstream output;
	output << "[MASTER] Master (rank = " << MASTER_RANK << ") receive "
			<< nAllTasks << " initial tasks. ";
	output << "Number of tasks are from: ";

	for (int i = 0; i < size; i++) {
		output << nSubTasks[i] << " tasks from rank = " << i << ", ";
	}
	output << endl;
	cout << output.str();
}

/*
 * Master print out the information of the assigned task
 */
void printMasterAssignedTask(int slaveRank, Task task) {
	stringstream output;
	output << "[MASTER] Master (rank = " << MASTER_RANK
			<< ") assign a task (owner = " << task.owner << ", index = "
			<< task.index << ") to slave (rank = " << slaveRank << ")." << endl;
	cout << output.str();
}

/*
 * Slave print out the information of the received task
 */
void printSlaveReceivedTask(int slaveRank, Task task) {
	stringstream output;
	output << "[SLAVE" << slaveRank << "] Slave (rank = " << slaveRank
			<< ") receive a task (owner = " << task.owner << ", index = "
			<< task.index << ") from Master." << endl;
	cout << output.str();
}

/*
 * Slave print out the information of the finished task
 */
void printSlaveFinishATask(int slaveRank, Task task, clock_t startTime) {
	clock_t endTime;
	endTime = clock();
	double diff = ((double) (endTime - startTime)) / CLOCKS_PER_SEC * 1000;

	stringstream output;
	output << "[SLAVE" << slaveRank << "] Slave (rank = " << slaveRank
			<< ") finish a task (owner = " << task.owner << ", index = "
			<< task.index << ", input = " << task.input << ", result = "
			<< task.result << ") in " << diff << "seconds." << endl;
	cout << output.str();
}

/*
 * Slave print out the information of the received task
 */
void printSlaveFinishAllTask(int slaveRank) {
	stringstream output;
	output << "[SLAVE" << slaveRank << "] Slave (rank = " << slaveRank
			<< ") finish all tasks assigned and do not have pending tasks. "
			<< "Will wait for master to send back related results." << endl;
	cout << output.str();
}

/*
 * Master print out that all the tasks have been finished
 */
void printMasterFinishAllTask() {
	stringstream output;
	output << "[MASTER] Master (rank = " << MASTER_RANK
			<< ") finish all tasks received. "
			<< "Will send back result to the related slaves. " << endl;
	cout << output.str();
}

/*
 * Print the final tasks of each process
 */
void printFinalSelfTasks(int rank, Task* selfTasks) {
	int nTasks = 0;
	for (int i = 0; i < MAX_TASKS; i++) {
		if (selfTasks[i].owner == -1) {
			break;
		}
		nTasks++;
	}
	stringstream output;
	output << "[Process] Process (rank = " << rank << ") has finished all "
			<< nTasks << " tasks. Input and result: ";

	for (int i = 0; i < MAX_TASKS; i++) {
		if (selfTasks[i].owner == -1) {
			break;
		}
		output << "(x = " << selfTasks[i].input << ", f(x) = "
				<< selfTasks[i].result << "), ";
	}
	output << endl;
	cout << output.str();
}

#endif /* HELPER_H_ */
