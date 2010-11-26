/*
 * FILE:		DataType.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef DATATYPE_H_
#define DATATYPE_H_

#include "samson.pb.h"		//ss::network::Data

class DataType
{
public:
	DataType(const QString &_name)
		: name(_name) {};
	~DataType(){};

	QString getName() { return name; };

	virtual void upload(ss::network::Data* data)
	{
		help = QString::fromStdString(data->help());
	}

protected:
	QString name;
	QString help;
};

#endif /* DATATYPE_H_ */
