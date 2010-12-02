/*
 * WorkspaceScene.h
 *
 *  Created on: Oct 18, 2010
 *      Author: ania
 */

#ifndef WORKSPACESCENE_H_
#define WORKSPACESCENE_H_

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QPointF>

#include "globals.h"

class QSvgRenderer;
class ConnectionItem;
class BaseItem;

class BaseObject;
class Queue;
class Operation;

class WorkspaceScene: public QGraphicsScene
{
	Q_OBJECT

public:
	WorkspaceScene(QObject* parent = 0);
	~WorkspaceScene();

	int getTool() {return current_tool; };

	void showQueue(Queue* queue, const QPointF &pos);
	void showOperation(Operation* operation, const QPointF &pos);
	void removeQueueItem(Queue* queue);					// Called from Workspace when deleting
														// queue request was finished successfully

public slots:
	void setTool(int tool) { current_tool = tool; };

	/*
	 * Move zooming to WorkspaceView class
	 */
	void zoomOut();
	void zoomReset();
	void zoomIn();

	void startConnection(BaseItem* item);
	void closeConnection(BaseItem* item);
	void cancelConnection();

signals:
	void addQueueRequested(const QPointF &);			// Emitted when user chooses to add queue.
														// Argument is the position of mouse click.
	void addOperationRequested(const QPointF &);		// Emitted when user chooses to add operation.
														// Argument is the position of mouse click.

	void infoRequested(BaseObject*);					// Emitted when user wants to get information about queue or operation
	void removeItemRequested(BaseItem*);				// Emitted when user wants to remove item from workspace
	void deleteQueueRequested(Queue*);					// Emitted when user chooses to delete queue from system
	void uploadDataRequested(Queue*);
	void downloadDataRequested(Queue*);

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	BaseItem* findItem(const QPointF &pos);

protected:
	static QSvgRenderer* queue_renderer;
	static QSvgRenderer* operation_renderer;
    int current_tool;
    ConnectionItem* current_conn;
};


#endif /* WORKSPACESCENE_H_ */
