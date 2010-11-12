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

class Operation;
class Queue;

class Process : public QObject
{
	Q_OBJECT

public:
	Process(Operation* _operation);
	~Process() {};

public slots:
	QString addInput(Queue* queue);
	QString addOutput(Queue* queue);

	void run() {};

public:
	Operation* operation;
	QVector<Queue*>* input;
	QVector<Queue*>* output;
};


#endif /* PROCESS_H_ */
