/*
 * FILE:		Process.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include "Process.h"
#include "OperationItem.h"
#include "Operation.h"
#include "QueueItem.h"
#include "Queue.h"

/*
 * Validate if type of queue's key-value are the same as required by operation.
 * If key-value are correct, connect queue to operation as input queue.
 * Return null string if the queue was added successfully, otherwise return error message.
 */
QString Process::addInput(QueueItem* queue_item)
{
	// We are assuming that input queues have to be added in the correct order - as defined in the operation.
	int input_number = input.size()+1;
	QList<KVPair*> inputs = operation_item->operation->getInput();
	if(inputs.size()<input_number)
		return QString("All queues are already connected.");

	KVPair* next = inputs[input_number-1];

	if (next->getKey()==queue_item->queue->getKey() &&
			next->getValue()==queue_item->queue->getValue() )
		input.append(queue_item);
	else
		return QString("Wrong queue. KV should be of type: %3 - %4").arg(next->getKey()).arg(next->getValue());

	return QString();
}

/*
 * Validate if type of queue's key-value are the same as required by operation.
 * If key-value are correct, connect queue to operation as output queue.
 * Return null string if the queue was added successfully, otherwise return error message.
 */
QString Process::addOutput(QueueItem* queue_item)
{
	// We are assuming that output queues have to be added in the correct order - as defined in the operation.
	int output_number = output.size()+1;
	QList<KVPair*> outputs = operation_item->operation->getOutput();
	if(outputs.size()<output_number)
		return QString("All queues are already connected.");

	KVPair* next = outputs[output_number-1];

	if (next->getKey()==queue_item->queue->getKey() &&
			next->getValue()==queue_item->queue->getValue() )
		output.append(queue_item);
	else
		return QString("Wrong queue. KV should be of type: %3 - %4").arg(next->getKey()).arg(next->getValue());

	return QString();
}
