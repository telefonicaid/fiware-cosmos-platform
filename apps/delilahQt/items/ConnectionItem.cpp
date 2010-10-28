/*
 * ConnectionItem.cpp
 *
 *  Created on: Oct 25, 2010
 *      Author: ania
 */

#include <iostream>
#include <cmath>

#include <QPainterPath>
#include <QPointF>
#include <QPainter>

#include "ConnectionItem.h"
#include "ObjectItem.h"

#define PI 3.14159265

ConnectionItem::ConnectionItem(QGraphicsItem* parent)
	: QGraphicsPathItem(parent)
{
	line_color = QColor(0, 0, 255);
	arrow_color = QColor(255, 0, 0);
	setZValue(-0.1);

	start_item = 0;
	end_item = 0;

//	path = QPainterPath(QPointF(0.0, 0.0));
	setPath(path);


	finished = false;
}

ConnectionItem::ConnectionItem(ObjectItem* start, ObjectItem* end, QGraphicsItem* parent)
	: QGraphicsPathItem(parent)
{
	line_color = QColor(0, 0, 255);
	arrow_color = QColor(255, 0, 0);
	setZValue(-0.1);

//	start_item = start;
	end_item = end;

	open(start);
	if (end)
		close(end);

//	if (end_item)
//	{
//		finished = true;
//	}
//	else
//	{
//		finished = false;
//		start_scene_pos = start->mapToScene(start->boundingRect().center());
//
//		path = QPainterPath(QPointF(0.0, 0.0));
//		setPath(path);
//
//		setPos(start_scene_pos);
//	}

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
		painter->setPen( QPen(QBrush(QColor(arrow_color)), 2, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin) );
		painter->setBrush(arrow_color);
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

bool ConnectionItem::open(ObjectItem* start)
{
	start_item = start;
	start_scene_pos = start->mapToScene(start->boundingRect().center());

	path = QPainterPath(QPointF(0.0, 0.0));
	setPos(start_scene_pos);

	finished = false;

	return true;
}

bool ConnectionItem::close(ObjectItem* end)
{
	// TODO: validate that it can be closed
	finished = true;

	if(finished)
	{
		QPainterPath arrow(QPointF(0, 0));

		QPointF head_point = mapFromItem(end, end->boundingRect().center());
		QPointF center = head_point/2;

		QRectF arrow_rect(center-QPointF(15.0, 15.0), QSizeF(30, 30));
		double angle = atan2(head_point.y(), head_point.x())*180/PI;

		arrow.lineTo(head_point);
		arrow.moveTo(center);
		arrow.arcTo(arrow_rect, 150-angle, 60);
		arrow.closeSubpath();

		path = arrow;
	}
	else
		path = QPainterPath();
	setPath(path);

	return finished;
}
