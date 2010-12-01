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

QString Queue::getHTMLInfo()
{
	QList< QStringList > info;
	QStringList name_info = (QStringList() << "Name: " << name);
	info << name_info;
	if (type==KV_QUEUE)
	{
		QStringList key_info = (QStringList() << "Key type: " << key);
		QStringList value_info = (QStringList() << "Value type: " << value);
		info << key_info << value_info;
	}

	// TODO: check formating
	QString queue_size = QString::number(size/1024, 'f', 2) + " kb";
	if (type==KV_QUEUE)
		queue_size.append(QString(" in %1 KV pairs").arg(kv_number));
	QStringList size_info = (QStringList() << "Size: " << queue_size);
	info << size_info;

	QString text;
	text += "<table>";
	for(int i=0; i<info.size(); i++)
	{
		text += "<tr>";
		text += "<td><b>" + info[i][0] + "</b></td>";
		text += "<td>" + info[i][1] + "</td>";
		text += "</tr>";
	}
	text += "</table>";

	return text;
}

