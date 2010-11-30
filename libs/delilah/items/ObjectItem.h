/*
 * ProcessItem.h
 *
 *  Created on: Oct 22, 2010
 *      Author: ania
 */

#ifndef PROCESSITEM_H_
#define PROCESSITEM_H_

#include <QGraphicsSvgItem>
#include <QGraphicsSceneContextMenuEvent>

class ConnectionItem;
class ObjectTextItem;
class BaseObject;

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

	virtual void initText() = 0;

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
	void infoRequested(BaseObject*);

protected:
    QSize default_size;
    QList<ConnectionItem*> connections;

    ObjectTextItem* text_item;
};


/*
 * Class introduced only to be sure, that the text displayed on the queue item
 * does not receive any mouse events.
 * It should be possible to set it when creating QGraphicsTextItem with
 * setAcceptedMouseButtons(0),but for some reason it does not work
 */
class ObjectTextItem: public QGraphicsTextItem
{
public:
	ObjectTextItem(QGraphicsItem* parent=0)
		: QGraphicsTextItem(parent) {};
	ObjectTextItem(const QString &text, QGraphicsItem* parent=0)
		: QGraphicsTextItem(text, parent) {};
	~ObjectTextItem() {};

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
	{
		event->ignore();
	};

};

#endif /* PROCESSITEM_H_ */
