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
	// TODO
	return QString("Here will be queue info");

//	QString number;		//helper
//
//	QList< QStringList > info;
//	QStringList name = (QStringList() << "Name: " << queue->getName());
//	// TODO: formating
//	QStringList size = (QStringList() << "Size: " << number.setNum(queue->getSize()/1024) + " kb");
//	info << name << size;
//	if (queue->getType()==KV_QUEUE)
//	{
//		QStringList key = (QStringList() << "Key type: " << queue->getKeyType());
//		QStringList value = (QStringList() << "Value type: " << queue->getValueType());
//		QStringList kv_number = (QStringList() << "Number of KV pairs:" << number.setNum(queue->getKVNumber()));
//		info << key << value << kv_number;
//	}
//
//	text += "<table>";
//	for(int i=0; i<info.size(); i++)
//	{
//		text += "<tr>";
//		text += "<td><b>" + info[i][0] + "</b></td>";
//		text += "<td>" + info[i][1] + "</td>";
//		text += "</tr>";
//	}
//	text += "</table>";
}

