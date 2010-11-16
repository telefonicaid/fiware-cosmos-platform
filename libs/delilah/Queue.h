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
//#include "globals.h"


class DataQueue : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(unsigned long size READ size WRITE setSize)

public:
	DataQueue() {};
	DataQueue(const QString &name) { setName(name); };
	// copy constructor - needed to declare DataQueue as new metatype
	// Not sure if it's needed. Probably will be removed.
	DataQueue(const DataQueue &q)
		: QObject()
	{
		_name = q.name();
		_size = q.size();
	};
	~DataQueue() {};

	/*
	 * Properties
	 */
	QString name() const { return _name; };
	void setName(const QString &name) { _name = name; };
	unsigned long size() const { return _size; };
	void setSize(const unsigned long &size) { _size = size; };

	// Operator to compare two queues - they are the same if they have the same name.
	virtual bool operator==(const DataQueue& q) const
	{
		QString name = q.name();
		if ( name.compare(_name) == 0 )
			return true;
		else
			return false;
	};

private:
	QString _name;
	unsigned long _size;
};

/*
 * KVQueue class
 * Represents Key-Value Queue in SAMSON
 */
class KVQueue : public DataQueue
{
	Q_OBJECT
	Q_PROPERTY(QString key READ key WRITE setKey)
	Q_PROPERTY(QString value READ value WRITE setValue)

public:
	KVQueue(const QString &name)
		: DataQueue(name) {};
	KVQueue(const QString &name, const QString &key, const QString &value)
		: DataQueue(name) { setKey(key); setValue(value); };
	~KVQueue(){};

	QString key() const { return _key; };
	void setKey(const QString &key) { _key = key; };
	QString value() const { return _value; };
	void setValue(const QString &value) { _value = value; };

	unsigned long getKVNumber() { return kv_number; };
	void setKVNumber(unsigned long num) { kv_number = num; };
private:
	QString _key;
	QString _value;
	unsigned long kv_number;
};

Q_DECLARE_METATYPE(DataQueue)

#endif /* QUEUE_H_ */
