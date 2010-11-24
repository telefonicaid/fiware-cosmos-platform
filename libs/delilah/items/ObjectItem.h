/*
 * ProcessItem.h
 *
 *  Created on: Oct 22, 2010
 *      Author: ania
 */

#ifndef PROCESSITEM_H_
#define PROCESSITEM_H_

#include <QGraphicsSvgItem>

class ConnectionItem;

class ObjectItem : public QGraphicsSvgItem
{
	Q_OBJECT

public:
	ObjectItem(QGraphicsItem* parent=0)
		: QGraphicsSvgItem(parent) { init(); };
	ObjectItem(QSvgRenderer* renderer, QGraphicsItem* parent=0);

	virtual ~ObjectItem();

	// Overloaded
	void setPos(const QPointF &pos);
	void setPos(qreal x,qreal y);

	QSize defaultSize() {return default_size; };
	virtual void setDefaultSize();
	virtual void setSize(QSize size);

	virtual void addConnection(ConnectionItem* connection) { connections.append(connection); };
	virtual bool isConnected(ObjectItem* item);

protected:
	virtual void init();
	virtual void initializeDefaultSize() = 0;

	virtual void putOnTop() { setZValue(qreal(0.1)); };
    virtual void restoreOrder() { setZValue(qreal(0)); };

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

signals:
	void posChanged();

protected:
    QSize default_size;
    QList<ConnectionItem*> connections;

};

#endif /* PROCESSITEM_H_ */
