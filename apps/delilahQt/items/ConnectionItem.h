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

class ConnectionItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:
	ConnectionItem(QGraphicsItem* parent=0);
	ConnectionItem(ObjectItem* start, ObjectItem* end=0, QGraphicsItem* parent=0);
	~ConnectionItem();

	bool open(ObjectItem* item);
	bool close(ObjectItem* item);
	void updateItem();

	ObjectItem* startItem() { return start_item; };
	ObjectItem* endItem() { return end_item; };

public slots:
	void updateStartPos();
	void updateEndPos();
	void updateEndPos(const QPointF &end_pos);

protected:
	void init();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

	virtual QPainterPath drawLine(const QPointF &start_pos, const QPointF &end_pos);
	// TODO: define default size outside
	virtual QPainterPath drawArrow(const QPointF &start_pos, const QPointF &end_pos, const QSizeF &head_size = QSizeF(30, 30));

protected:
	bool finished;
	QPainterPath path;

	ObjectItem* start_item;
	ObjectItem* end_item;
	QPointF scene_start_pos;
	QPointF scene_end_pos;

	QColor line_color;
	QColor arrow_color;
};

#endif /* CONNECTIONITEM_H_ */
