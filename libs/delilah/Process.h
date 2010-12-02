/*
 * FILE:		Process.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <QObject>

class OperationItem;
class QueueItem;

class Process : public QObject
{
	Q_OBJECT

public:
	Process(OperationItem* operation)
		: operation_item(operation) {};
	~Process() {};

public slots:
	QString addInput(QueueItem* queue_item);
	QString addOutput(QueueItem* queue_item);

	void run() {};

public:
	OperationItem* operation_item;
	QList<QueueItem*> input;
	QList<QueueItem*> output;
};


#endif /* PROCESS_H_ */
