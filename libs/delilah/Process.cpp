/*
 * FILE:		Process.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include "Process.h"
#include "Operation.h"
#include "Queue.h"

Process::Process(Operation* _operation)
{
	operation = _operation;

	// TODO:
	// Check number of inputs and outputs of operation and initialize
	// vectors input and output with appropriate size
}

/*
 * Validate if type of queue's key-value are the same as required by operation.
 * If key-value are correct, connect queue to operation as input queue.
 * Return null string if the queue was added successfully, otherwise return error message.
 */
QString Process::addInput(Queue* queue)
{
	// TODO: Implement me!!!!!
	return QString();
}

/*
 * Validate if type of queue's key-value are the same as required by operation.
 * If key-value are correct, connect queue to operation as output queue.
 * Return null string if the queue was added successfully, otherwise return error message.
 */
QString Process::addOutput(Queue* queue)
{
	// TODO: Implement me!!!!!
	return QString();
}
