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

class Queue : public QObject
{
	Q_OBJECT

public:
	Queue()
		: status(Queue::LOADING) {};
	Queue(const QString &name)
		: status(Queue::LOADING), name(name) {};
	~Queue() {};

	/*
	 * Status property
	 */
	enum Status {LOADING, READY, DELETED};
	Status getStatus() const { return status; };
	void setStatus(Status new_status)
	{
		if (status!=new_status)
		{
			status=new_status;
			emit(statusChanged());
		}
	};

	/*
	 *
	 */
	virtual QueueType type() = 0;


	QString getName() const { return name; };
	unsigned long getSize() const { return size; };

signals:
	void statusChanged();

protected:
	Status status;
	QString name;
	unsigned long size;
};


/*
 * DataQueue class
 * Represents Data Queue in SMASON
 */
class DataQueue : public Queue
{
public:
	DataQueue(const QString &name)
		: Queue(name) {};
	~DataQueue() {};

	virtual QueueType type() { return DATA_QUEUE; };

	/*
	 * Sets queue variables to the ones got from the SAMSON platform. At the end,
	 * set status to READY.
	 * Returns current status of the queue.
	 */
	int upload(ss::network::DataQueue* q)
	{
		size = q->size();

		setStatus(DataQueue::READY);
		return status;
	}
};


/*
 * KVQueue class
 * Represents Key-Value Queue in SAMSON
 */
class KVQueue : public DataQueue
{
public:
	KVQueue(const QString &name)
		: DataQueue(name) {};
	KVQueue(const QString &name, const QString &_key, const QString &_value)
		: DataQueue(name), key(_key), value(_value) {};
	~KVQueue(){};

	virtual QueueType type() { return KV_QUEUE; };

//	QString getKey() const { return key; };
//	void setKey(const QString &_key) { key = _key; };
//	QString getValue() const { return value; };
//	void setValue(const QString &_value) { value = _value; };
//	unsigned long getKVNumber() { return kv_number; };
//	void setKVNumber(unsigned long num) { kv_number = num; };

	/*
	 * Sets queue variables to the ones got from the SAMSON platform. At the end,
	 * set status to READY.
	 * Returns current status of the queue.
	 */
	int upload(ss::network::Queue* q)
	{
		size = q->info().size();
		key = QString::fromStdString(q->format().keyformat());
		value = QString::fromStdString(q->format().valueformat());
		kv_number = q->info().kvs();

		setStatus(Queue::READY);
		return status;
	}

protected:
	QString key;
	QString value;
	unsigned long kv_number;
};

#endif /* QUEUE_H_ */
