/*
 * ConnectionItem.cpp
 *
 *  Created on: Oct 25, 2010
 *      Author: ania
 */

#include <iostream>

#include <QPainterPath>
#include <QPointF>
#include <QPainter>

#include "ConnectionItem.h"
#include "ObjectItem.h"

ConnectionItem::ConnectionItem(QGraphicsItem* parent)
	: QGraphicsPathItem(parent)
{
	start_item = 0;
	end_item = 0;

	path = QPainterPath(QPointF(0.0, 0.0));
	setPath(path);

	line_color = QColor(0, 0, 255);
	arrow_color = QColor(255, 0, 0);

	finished = false;
}

ConnectionItem::ConnectionItem(ObjectItem* start, ObjectItem* end, QGraphicsItem* parent)
	: QGraphicsPathItem(parent)
{
	line_color = QColor(0, 0, 255);
	arrow_color = QColor(255, 0, 0);
	start_item = start;
	end_item = end;

	if (end_item)
	{
		finished = true;
	}
	else
	{
		finished = false;
		start_scene_pos = start->mapToScene(start->boundingRect().center());

		path = QPainterPath(QPointF(0.0, 0.0));
		setPath(path);

		setPos(start_scene_pos);
	}

}

ConnectionItem::~ConnectionItem()
{

}

void ConnectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if(!finished)
	{
		painter->setPen( QPen(QBrush(QColor(line_color)), 1, Qt::DashLine, Qt::SquareCap, Qt::BevelJoin) );
	}
	else
	{
		painter->setPen( QPen(QBrush(QColor(arrow_color)), 3, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin) );
	}

	painter->drawPath(path);
}

void ConnectionItem::drawLine(const QPointF &pos)
{
	QPainterPath line(QPointF(0.0, 0.0));
	QPointF move = pos - start_scene_pos;
	line.lineTo(move);

	path = line;
	setPath(path);
}

bool ConnectionItem::close(ObjectItem* end)
{
	// TODO: validate that it can be closed
	finished = true;

	if(finished)
	{
		QPainterPath arrow(QPointF(0, 0));
		arrow.lineTo(mapFromItem(end, end->boundingRect().center()));

		path = arrow;
	}
	else
		path = QPainterPath();
	setPath(path);

	return finished;
}
