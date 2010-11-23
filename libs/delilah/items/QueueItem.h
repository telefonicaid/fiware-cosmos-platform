#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <QGraphicsSceneContextMenuEvent>

#include "ObjectItem.h"
#include "globals.h"

class Queue;
class DataQueue;
class QueueTextItem;

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

public:
    enum { Type=QUEUE_ITEM };
    Queue* queue;

private:
    QueueTextItem* text_item;
};

/*
 * Class introduced only to be sure, that the text displayed on the queue item
 * does not receive any mouse events.
 * It should be possible to set it when creating QGraphicsTextItem with
 * setAcceptedMouseButtons(0),but for some reason it does not work
 */
class QueueTextItem: public QGraphicsTextItem
{
public:
	QueueTextItem(QGraphicsItem* parent=0)
		: QGraphicsTextItem(parent) {};
	QueueTextItem(const QString &text, QGraphicsItem* parent=0)
		: QGraphicsTextItem(text, parent) {};
	~QueueTextItem() {};

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
	{
		event->ignore();
	};

};

#endif // QUEUEITEM_H
