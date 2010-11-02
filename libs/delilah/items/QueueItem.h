#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include "ObjectItem.h"
#include "globals.h"

class QueueItem : public ObjectItem
{
	Q_OBJECT

public:
	QueueItem()
		: ObjectItem() {};
	QueueItem(QSvgRenderer* renderer)
		: ObjectItem(renderer) {};
	QueueItem(const QString &fileName, QGraphicsItem *parent=0)
		: ObjectItem(fileName, parent) {};
	~QueueItem();

	virtual int type() const { return Type; };
	virtual void initText(QString text);

protected:
	virtual void init();
	virtual void initializeDefaultSize();

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

public:
    enum { Type=QUEUE_ITEM };

private:
    QGraphicsTextItem* text_item;
};

#endif // QUEUEITEM_H
