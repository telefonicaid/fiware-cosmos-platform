#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <QGraphicsSvgItem>
#include <QColor>

class QueueItem : public QGraphicsSvgItem
{
	Q_OBJECT

public:
	QueueItem();
	QueueItem(QSvgRenderer* renderer)
		{ setSharedRenderer(renderer); };
	QueueItem(const QString &fileName, QGraphicsItem *parent=0)
		: QGraphicsSvgItem(fileName, parent) {};
	~QueueItem();

	virtual void initText(QString text);
	virtual void scaleToDefaultSize();

	void setPos(const QPointF &pos);
	void setPos(qreal x,qreal y);

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

private:
    QSize default_size;
    QRectF* bound_rect;

protected:
    QGraphicsTextItem* text_item;

};

#endif // QUEUEITEM_H
