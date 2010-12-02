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
#include "BaseObject.h"

class KVPair;

class Operation : public BaseObject
{
public:
	Operation(const QString &name)
		: BaseObject(name) {};
	~Operation() {};

	void upload(ss::network::Operation* o);
	virtual QString getHTMLInfo();

	QList<KVPair*> getInput() { return input; };
	QList<KVPair*> getOutput() { return output; };

protected:
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

