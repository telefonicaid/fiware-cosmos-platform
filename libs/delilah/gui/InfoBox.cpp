/*
 * FILE:		InfoBox.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include "InfoBox.h"
#include "Queue.h"

InfoBox::InfoBox(QWidget *parent)
    : QDialog(parent, Qt::CustomizeWindowHint)
{
	ui.setupUi(this);

}

void QueueInfoBox::setInfo()
{
	QString text;
	if(queue==0)
		text = "<FONT color=red><b>Error</b>: No queue selected.</FONT>";
	else
	{
		QString number;		//helper

		QList< QStringList > info;
		QStringList name = (QStringList() << "Name: " << queue->getName());
//		// TODO: formating
		QStringList size = (QStringList() << "Size: " << number.setNum(queue->getSize()) + " kb");
		info << name << size;
		if (queue->getType()==KV_QUEUE)
		{
			QStringList key = (QStringList() << "Key type: " << queue->getKeyType());
			QStringList value = (QStringList() << "Value type: " << queue->getValueType());
			QStringList kv_number = (QStringList() << "Number of KV pairs:" << number.setNum(queue->getKVNumber()));
			info << key << value << kv_number;
		}

		text += "<table>";
		for(int i=0; i<info.size(); i++)
		{
			text += "<tr>";
			text += "<td><b>" + info[i][0] + "</b></td>";
			text += "<td>" + info[i][1] + "</td>";
			text += "</tr>";
		}
		text += "</table>";
	}

	setHtml(text);
}

void QueueInfoBox::setInfo(Queue* _queue)
{
	queue = _queue;
	setInfo();
}
