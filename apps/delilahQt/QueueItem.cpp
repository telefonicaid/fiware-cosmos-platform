/*
 * QueueItem.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: ania
 */
#include <iostream>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QFont>

#include "QueueItem.h"


QueueItem::QueueItem()
{
	// TODO: put it in some configuration
	default_size = QSize(100, 100);
	text_item = NULL;

	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

QueueItem::~QueueItem()
{
}

void QueueItem::initText(QString text)
{
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
}

void QueueItem::setPos(const QPointF &pos)
{
	QRectF bound_rect(QGraphicsSvgItem::boundingRect());
	QPointF new_pos = pos - mapToScene(bound_rect.center());

	QGraphicsSvgItem::setPos(new_pos);
}

void QueueItem::setPos(qreal x,qreal y)
{
	setPos(QPointF(x, y));
}


void QueueItem::scaleToDefaultSize()
{
	// Calculate scale factor
	QSizeF size = QRectF(QGraphicsSvgItem::boundingRect()).size();
	size.scale(default_size.width(), default_size.height(), Qt::KeepAspectRatio);
	qreal scale_factor = size.width() / QGraphicsSvgItem::boundingRect().width();

	prepareGeometryChange();
	setScale(scale_factor);
}

void QueueItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu* menu = new QMenu();
	menu->addAction("Show Queue Info");
	menu->exec(event->screenPos());
}
