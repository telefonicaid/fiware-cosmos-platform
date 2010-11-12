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

//#include "globals.h"


class Queue : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)

public:
	Queue(const QString &name) { setName(name); };
	~Queue() {};

	/*
	 * Properties
	 */
	QString name() const { return _name; };
	void setName(const QString &name) { _name = name; };

private:
	QString _name;
};

/*
 * DataQueue class
 * Represents Data (TXT) Queue in SAMSON
 */
class DataQueue : public Queue
{
public:
	DataQueue(const QString &name)
		: Queue(name) {};
	~DataQueue() {};

private:
	size_t size;
};

/*
 * KVQueue class
 * Represents Key-Value Queue in SAMSON
 */
class KVQueue : public Queue
{
public:
	KVQueue(const QString &name, const QString &key, const QString &value)
		: Queue(name) { _key = key; _value = value; };
	~KVQueue(){};

private:
	QString _key;
	QString _value;
};

#endif /* QUEUE_H_ */
