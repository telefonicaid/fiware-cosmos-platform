#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include "ObjectItem.h"
#include "globals.h"

class Queue;

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

	void showQueueInfoSelected();
	void removeQueueSelected();
	void deleteQueueSelected();
	void loadDataSelected();

protected:
	virtual void init();
	virtual void initializeDefaultSize();

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

signals:
	void removeQueueFromWorkspaceRequested(Queue*);			// emitted when user chooses to remove queue from workspace
	void deleteQueueRequested(Queue*);						// emitted when user chooses to delete queue from system
	void queueInfoRequested(Queue*);

public:
    enum { Type=QUEUE_ITEM };
    Queue* queue;
};


#endif // QUEUEITEM_H
