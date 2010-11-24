/*
 * FILE:		InfoBox.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include "InfoBox.h"

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
		// TODO:
		text = "Here will be queue info";
	}

	setHtml(text);
}

void QueueInfoBox::setInfo(Queue* _queue)
{
	queue = _queue;
	setInfo();
}
