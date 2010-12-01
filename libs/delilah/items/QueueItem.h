#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include "BaseItem.h"
#include "globals.h"

class Queue;

class QueueItem : public BaseItem
{
	Q_OBJECT

public:
	QueueItem()
		: BaseItem() { init(); };
	QueueItem(QSvgRenderer* renderer)
		: BaseItem(renderer) { init(); };
	QueueItem(Queue* _queue);
	~QueueItem() {};

	virtual int type() const { return Type; };
	virtual void initText();
	virtual void initText(QString text);

public slots:
	void updateItem();

	void showInfoSelected();
	void removeQueueSelected();
	void deleteQueueSelected();
	void uploadDataSelected();
	void downloadDataSelected();

protected:
	virtual void init();
	virtual void initializeDefaultSize();

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

signals:
	void deleteQueueRequested(Queue*);						// emitted when user chooses to delete queue from system
	void uploadDataRequested(Queue*);

public:
    enum { Type=QUEUE_ITEM };
    Queue* queue;
};


#endif // QUEUEITEM_H
