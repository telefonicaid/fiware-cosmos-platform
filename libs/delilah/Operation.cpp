/*
 * FILE:		Operation.cpp
 *
 * AUTHOR:		ania
 *
 * DESCRIPTION:
 *
 */

#ifndef OPERATION_CPP_
#define OPERATION_CPP_

#include "Operation.h"

void Operation::upload(ss::network::Operation* o)
{
	help = QString::fromStdString(o->help());
	help_line = QString::fromStdString(o->help_line());

	for(int i=0; i< o->input_size(); i++)
	{
		ss::network::KVFormat format = o->input(i);
		QString key = QString::fromStdString(format.keyformat());
		QString value = QString::fromStdString(format.valueformat());
		KVPair* kv_pair = new KVPair(key, value);
		input.append(kv_pair);
	}

	for(int i=0; i<o->output_size(); i++)
	{
		ss::network::KVFormat format = o->output(i);
		QString key = QString::fromStdString(format.keyformat());
		QString value = QString::fromStdString(format.valueformat());
		KVPair* kv_pair = new KVPair(key, value);
		output.append(kv_pair);
	}
}

#endif /* OPERATION_CPP_ */
