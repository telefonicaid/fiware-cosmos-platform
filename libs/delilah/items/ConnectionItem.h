/*
 * ConnectionItem.h
 *
 *  Created on: Oct 25, 2010
 *      Author: ania
 */

#ifndef CONNECTIONITEM_H_
#define CONNECTIONITEM_H_

#include <QGraphicsPathItem>

class BaseItem;

class ConnectionItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:
	ConnectionItem(QGraphicsItem* parent=0);
	ConnectionItem(BaseItem* start, BaseItem* end=0, QGraphicsItem* parent=0);
	~ConnectionItem();

	bool open(BaseItem* item);
	bool close(BaseItem* item);
	void updateItem();

	BaseItem* startItem() { return start_item; };
	BaseItem* endItem() { return end_item; };

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

	BaseItem* start_item;
	BaseItem* end_item;
	QPointF scene_start_pos;
	QPointF scene_end_pos;

	QColor line_color;
	QColor arrow_color;
};

#endif /* CONNECTIONITEM_H_ */
