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
	: ObjectItem(), queue(_queue)
{
	init();

	connect(queue, SIGNAL(statusChanged()), this, SLOT(updateItem()));
}

void QueueItem::init()
{
	initializeDefaultSize();
	text_item = 0;
}

void QueueItem::initializeDefaultSize()
{
	// TODO:
	default_size = QSize(100, 100);
}

void QueueItem::initText()
{
	if(queue==0)
		return;

	QString text;
	if (queue->getStatus()==Queue::LOADING)
		text = QString("Loading...");
	if(queue->getStatus()==Queue::READY)
		text = queue->getName();

	initText(text);
}

void QueueItem::initText(QString text)
{
	if (text_item!=0)
		delete text_item;

	text_item = new QGraphicsTextItem(text, this);
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

//	text_item->setEnabled(false);
	text_item->setAcceptedMouseButtons(0);
//	text_item->setActive(false);
//	text_item->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

void QueueItem::updateItem()
{
	initText();
}

void QueueItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu* menu = new QMenu();
	menu->addAction("Show Queue Info", this, SLOT(showInfoBox()));
	menu->exec(event->screenPos());
}

