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
class ObjectItem;

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
	void removeQueue(Queue* queue);
	void showOperation(Operation* operation, const QPointF &pos);

public slots:
	void setTool(int tool) { current_tool = tool; };

	/*
	 * Move zooming to WorkspaceView class
	 */
	void zoomOut();
	void zoomReset();
	void zoomIn();

	void startConnection(ObjectItem* item);
	void closeConnection(ObjectItem* item);
	void cancelConnection();

signals:
	void addQueueRequested(const QPointF &);			// Emitted when user chooses to add queue.
														// Argument is the position of mouse click.
	void addOperationRequested(const QPointF &);		// Emitted when user chooses to add operation.
														// Argument is the position of mouse click.

	void removeQueueFromWorkspaceRequested(Queue*);		// Emitted when user chooses to remove queue from workspace
	void deleteQueueRequested(Queue*);					// Emitted when user chooses to delete queue from system
	void queueInfoRequested(Queue*);					// Emitted when user wants to get information about queue

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	ObjectItem* findItem(const QPointF &pos);


protected:
	static QSvgRenderer* queue_renderer;
	static QSvgRenderer* operation_renderer;
    int current_tool;
    ConnectionItem* current_conn;
};


#endif /* WORKSPACESCENE_H_ */
