/*
 * FILE:		Operation.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef OPERATION_H_
#define OPERATION_H_

#include <QObject>

#include "samson.pb.h"		//ss::network::Operation

class KVPair;

class Operation : public QObject
{
	Q_OBJECT

public:
	Operation(const QString &_name)
		: name(_name) {};
	~Operation() {};

	QString getName() { return name; };

	void upload(ss::network::Operation* o);

//	void addInput(KVPair* kv_pair)
//	{
//		input.append(kv_pair);
//	};
//	void addInput(QString key, QString value)
//	{
//		KVPair kv_pair = new KVPair(key, value);
//		input.append(kv_pair);
//	};
//
//	void addOutput(KVPair* kv_pair)
//	{
//		output.append(kv_pair);
//	};
//	void addOutput(QString key, QString value)
//	{
//		KVPair kv_pair = new KVPair(key, value);
//		output.append(kv_pair);
//	};

protected:
	QString name;

	QList<KVPair*> input;
	QList<KVPair*> output;

	QString help;
	QString help_line;
};


class KVPair
{
public:
	KVPair(const QString &_key, const QString &_value)
		: key(_key), value(_value) {};
	~KVPair() {};

	QString getKey() { return key; };
	QString getValue() { return value; };

protected:
	QString key;
	QString value;
};

#endif /* OPERATION_H_ */

