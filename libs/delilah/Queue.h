/*
 * FILE:		Queue.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:	Reprezentation of Samson Queue
 *
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <QObject>
#include <QMetaType>

#include "samson.pb.h"		//ss::network::Queue, ss::network::DataQueue
#include "globals.h"


#define DATA_QUEUE_KV_FORMAT		"txt"

class Queue : public QObject
{
	Q_OBJECT

public:
//	Queue()
//		: type(Queue::KV_QUEUE), status(Queue::SYNCHRONIZING) { };
	Queue(const QString &_name, const QueueType _type=KV_QUEUE)
		: status(Queue::SYNCHRONIZING), type(_type), name(_name) {};
	~Queue() {};

	QueueType getType() { return type; };
	QString getName() const { return name; };
	unsigned long getSize() const { return size; };
	QString getKeyType() const { return key; }
	QString getValueType() const { return value; }
	unsigned long getKVNumber() const { return kv_number; }

	/*
	 * Status property
	 */
	enum Status {SYNCHRONIZING, READY, DELETED};
	Status getStatus() const { return status; };
	void setStatus(Status new_status)
	{
		if (status!=new_status)
		{
			status=new_status;
			emit(statusChanged());
		}
	};

	virtual int upload(ss::network::Queue* q);

signals:
	void statusChanged();

protected:
	Status status;
	QueueType type;
	QString name;
	unsigned long size;

	// Values used only for Key-Value Queue
	QString key;
	QString value;
	unsigned long kv_number;
};


#endif /* QUEUE_H_ */
