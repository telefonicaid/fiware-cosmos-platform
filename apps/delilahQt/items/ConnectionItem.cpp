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
	: QObject(), QGraphicsPathItem(parent)
{
	init();
	start_item = 0;
	end_item = 0;

	setPath(path);
}

ConnectionItem::ConnectionItem(ObjectItem* start_item, ObjectItem* end_item, QGraphicsItem* parent)
	: QObject(), QGraphicsPathItem(parent)
{
	init();

	open(start_item);
	if (end_item)
		close(end_item);
}

void ConnectionItem::init()
{
	line_color = QColor(0, 0, 255);
	arrow_color = QColor(255, 0, 0);
	setZValue(-0.1);
	finished = false;
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

void ConnectionItem::updateStartPos()
{
	std::cout << "updating start position\n";
	if (!start_item)
	{
		// TODO:
		std::cout << "Error - start item is not defined\n";
		return;
	}

	scene_start_pos = start_item->mapToScene(start_item->boundingRect().center());
	updateItem();
}

void ConnectionItem::updateEndPos()
{
	std::cout << "updating end position\n";
	if (!end_item)
	{
		// TODO:
		std::cout << "Error - end item is not defined\n";
		return;
	}

	scene_end_pos = end_item->mapToScene(end_item->boundingRect().center());
	updateItem();
}

void ConnectionItem::updateEndPos(const QPointF &pos)
{
	scene_end_pos = pos;
	updateItem();
}

void ConnectionItem::updateItem()
{
	if (finished)
		path = drawArrow(scene_start_pos, scene_end_pos);
	else
		path = drawLine(scene_start_pos, scene_end_pos);

	setPath(path);
}

QPainterPath ConnectionItem::drawLine(const QPointF &start_pos, const QPointF &end_pos)
{
	setPos(start_pos);

	QPainterPath line(QPointF(0.0, 0.0));
	QPointF move = end_pos - start_pos;
	line.lineTo(move);

	return line;
}

QPainterPath ConnectionItem::drawArrow(const QPointF &start_pos, const QPointF &end_pos, const QSizeF &head_size)
{
	setPos(start_pos);

	//Helpers - we assume the arrow's starting point is in (0, 0);
	QPointF start(0, 0);
	QPointF end = mapFromScene(end_pos);
	QPointF center = end/2;

	QRectF arrow_rect(center-QPointF(head_size.width()/2, head_size.height()/2), head_size);
	double angle = atan2(end.y(), end.x())*180/PI;

	QPainterPath arrow(start);
	arrow.lineTo(end);
	arrow.moveTo(center);
	arrow.arcTo(arrow_rect, 150-angle, 60);
	arrow.closeSubpath();

	return arrow;
}

bool ConnectionItem::open(ObjectItem* item)
{
	start_item = item;
	scene_start_pos = start_item->mapToScene(start_item->boundingRect().center());

	return true;
}

bool ConnectionItem::close(ObjectItem* item)
{
	// TODO: validate that it can be closed
	finished = true;

	if(finished)
	{
		end_item = item;
		updateEndPos();

		connect(start_item, SIGNAL(posChanged()), this, SLOT(updateStartPos()));
		connect(end_item, SIGNAL(posChanged()), this, SLOT(updateEndPos()));
	}

	return finished;
}
