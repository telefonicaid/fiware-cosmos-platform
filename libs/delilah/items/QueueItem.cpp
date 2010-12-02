/*
 * QueueItem.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: ania
 */
#include <iostream>

#include <QPainter>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QFont>

#include "QueueItem.h"
#include "Queue.h"

QueueItem::QueueItem(Queue* _queue)
	: BaseItem(), queue(_queue)
{
	init();

	connect(queue, SIGNAL(statusChanged()), this, SLOT(updateItem()));
}

void QueueItem::init()
{
	initializeDefaultSize();
}

void QueueItem::initializeDefaultSize()
{
	default_size = QSize(142, 94);
}

void QueueItem::initText()
{
	if(queue==0)
		return;

	QString text;
	if (queue->getStatus()==Queue::SYNCHRONIZING)
		text = QString("Synchronizing...");
	if (queue->getStatus()==Queue::UPLOADING)
		text = QString("Uploading...");
	if(queue->getStatus()==Queue::READY)
		text = queue->getName();
	if(queue->getStatus()==Queue::DELETED)
		text = QString("Deleted!!!");

	initText(text);
}

void QueueItem::initText(QString text)
{
	if (text_item!=0)
		delete text_item;

	text_item = new ObjectTextItem(text, this);
	// Don't know why this dosn't work...
//	text_item->setAcceptedMouseButtons(0);
	QFont serifFont("Times", 12, QFont::Bold);
	text_item->setFont(serifFont);

	QRectF item_rect(boundingRect());
	QRectF text_rect(text_item->boundingRect());

	// scale
	qreal scale_factor = 0.8*item_rect.width()/text_rect.width();
	text_item->setScale(scale_factor);

	// move to center
	QPointF p = item_rect.center() - text_item->mapToParent(text_rect.center());
	text_item->moveBy(p.x(), p.y());
}

void QueueItem::updateItem()
{
	initText();
}

void QueueItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu* menu = new QMenu();
	menu->addAction("Show Queue Info", this, SLOT(showInfoSelected()));
	menu->addAction("Remove Queue (from workspace)", this, SLOT(removeQueueSelected()));
	menu->addAction("Delete Queue", this, SLOT(deleteQueueSelected()));

	if(queue->getType()==DATA_QUEUE)
	{
		menu->addAction("Upload Data", this, SLOT(uploadDataSelected()));
		menu->addAction("Download Data", this, SLOT(downloadDataSelected()));
	}
	menu->exec(event->screenPos());
}

void QueueItem::showInfoSelected()
{
	emit(infoRequested(queue));
}

void QueueItem::removeQueueSelected()
{
	emit(removeItemRequested(this));
}

void QueueItem::deleteQueueSelected()
{
	emit(deleteQueueRequested(this->queue));
}

void QueueItem::uploadDataSelected()
{
	emit(uploadDataRequested(this->queue));
}

void QueueItem::downloadDataSelected()
{
	emit(downloadDataRequested(this->queue));
}
