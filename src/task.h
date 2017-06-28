/*
 * task.h
 *
 *  Created on: Jun 27, 2017
 *      Author: yu
 */

#ifndef TASK_H_
#define TASK_H_

using namespace std;

#include <cstdlib>
#include <stddef.h>

/**
 * The struct task serve as a data structure
 */
struct Task {
	Task() :
			owner(-1), index(-1), input(0.0), result(0.0) {
	}
	;
	Task(int iOwner, int iIndex, double iInput, double iResult) :
			owner(iOwner), index(iIndex), input(iInput), result(iResult) {
	}
	;
	Task(Task& newTask) :
			owner(newTask.owner), index(newTask.index), input(newTask.input), result(
					newTask.result) {
	}
	;
	int owner;
	int index;
	double input;
	double result;
};

#endif /* TASK_H_ */
