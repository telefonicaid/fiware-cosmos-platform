/*
 * ProcessItem.h
 *
 *  Created on: Oct 22, 2010
 *      Author: ania
 */

#ifndef PROCESSITEM_H_
#define PROCESSITEM_H_

#include <QGraphicsSvgItem>


class ObjectItem : public QGraphicsSvgItem
{
public:
	ObjectItem(QGraphicsItem* parent=0)
		: QGraphicsSvgItem(parent) { init(); };
	ObjectItem(const QString &fileName, QGraphicsItem* parent=0)
		: QGraphicsSvgItem(fileName, parent) { init(); };
	ObjectItem(QSvgRenderer* renderer, QGraphicsItem* parent=0);

	virtual ~ObjectItem();

	// Overloaded
	void setPos(const QPointF &pos);
	void setPos(qreal x,qreal y);

	QSize defaultSize() {return default_size; };
	virtual void setDefaultSize();
	virtual void setSize(QSize size);

protected:
	virtual void init();
	virtual void initializeDefaultSize() = 0;

	virtual void putOnTop() { setZValue(qreal(0.1)); };
    virtual void restoreOrder() { setZValue(qreal(0)); };

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

protected:
    QSize default_size;
};

#endif /* PROCESSITEM_H_ */
