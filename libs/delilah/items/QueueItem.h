#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include "ObjectItem.h"
#include "globals.h"

class Queue;
class DataQueue;

class QueueItem : public ObjectItem
{
	Q_OBJECT

public:
	QueueItem()
		: ObjectItem() { init(); };
	QueueItem(QSvgRenderer* renderer)
		: ObjectItem(renderer) { init(); };
	QueueItem(Queue* _queue);
	~QueueItem() {};

	virtual int type() const { return Type; };
	virtual void initText();
	virtual void initText(QString text);

public slots:
	void updateItem();

protected:
	virtual void init();
	virtual void initializeDefaultSize();

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

public:
    enum { Type=QUEUE_ITEM };

private:
    QGraphicsTextItem* text_item;
    Queue* queue;
};


//class DataQueueItem : public QueueItem
//{
//public:
//	DataQueueItem()
//		: QueueItem() { queue=0; };
//	DataQueueItem(DataQueue* _queue)
//		: QueueItem(), queue(queue)
//	{
////		queue=_queue;
////		connect(queue, SIGNAL(changed()), this, SLOT(updateItem()))
//	};
//	~DataQueueItem() {};
//
//private:
//	DataQueue* queue;
//};

#endif // QUEUEITEM_H
