/*
 * FILE:		Queue.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include <iostream>

#include "Queue.h"

int Queue::upload(ss::network::Queue* q)
{
	size = q->info().size();

	QString key_format = QString::fromStdString(q->format().keyformat());
	QString value_format = QString::fromStdString(q->format().valueformat());
	if (key_format.compare(DATA_QUEUE_KV_FORMAT, Qt::CaseInsensitive) != 0 ||
			value_format.compare(DATA_QUEUE_KV_FORMAT, Qt::CaseInsensitive) != 0)
	{
		key = QString::fromStdString(q->format().keyformat());
		value = QString::fromStdString(q->format().valueformat());
		kv_number = q->info().kvs();
		type = KV_QUEUE;
	}
	else
	{
		key = QString();
		value = QString();
		kv_number = 0;
		type = DATA_QUEUE;
	}

	setStatus(Queue::READY);
	return status;
}

