/*
 * ConnectionItem.h
 *
 *  Created on: Oct 25, 2010
 *      Author: ania
 */

#ifndef CONNECTIONITEM_H_
#define CONNECTIONITEM_H_

#include <QGraphicsPathItem>

class ObjectItem;

class ConnectionItem : public QGraphicsPathItem
{
public:
	ConnectionItem(QGraphicsItem* parent=0);
	ConnectionItem(ObjectItem* start, ObjectItem* end=0, QGraphicsItem* parent=0);
	~ConnectionItem();

	void drawLine(const QPointF &pos);
	bool open(ObjectItem* start);
	bool close(ObjectItem* end);

	ObjectItem* startItem() { return start_item; };
	ObjectItem* endItem() { return end_item; };

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);


protected:
	QPainterPath path;

	ObjectItem* start_item;
	ObjectItem* end_item;
	QPointF start_scene_pos;
	QPointF end_scene_pos;

	bool finished;

	QColor line_color;
	QColor arrow_color;
};

#endif /* CONNECTIONITEM_H_ */
