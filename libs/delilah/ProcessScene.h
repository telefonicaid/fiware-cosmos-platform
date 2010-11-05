/*
 * ProcessScene.h
 *
 *  Created on: Oct 18, 2010
 *      Author: ania
 */

#ifndef PROCESSSCENE_H_
#define PROCESSSCENE_H_

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QPointF>
#include <QAction>

#include "globals.h"

class QSvgRenderer;
class ConnectionItem;
class ObjectItem;

class ProcessScene: public QGraphicsScene
{
	Q_OBJECT

public:
	ProcessScene(QObject* parent = 0);
	~ProcessScene();

	int getTool() {return current_tool; };

public slots:
	void setTool(int tool) { current_tool = tool; };
	void addQueue(const QPoint &position = QPoint(0.0, 0.0) );
	void addOperation(const QPointF &position = QPoint(0.0, 0.0));
	void zoomOut();
	void zoomReset();
	void zoomIn();

	void startConnection(ObjectItem* item);
	void closeConnection(ObjectItem* item);
	void cancelConnection();

signals:
	void addQueueRequested(const QPoint &);

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

class NewQueueAction : public QAction
{
	Q_OBJECT

public:
	NewQueueAction(QObject* parent) : QAction(parent)
		{ connect(this, SIGNAL(triggered()), this, SLOT(triggerAtPos()));};
	NewQueueAction(const QString & text, QObject* parent) : QAction(text, parent)
		{ connect(this, SIGNAL(triggered()), this, SLOT(triggerAtPos())); };
	~NewQueueAction() {};

	void setPosition(QPointF p) { pos=p; };
	QPointF position() { return pos; };

protected slots:
	void triggerAtPos() { emit(triggered(pos)); };

signals:
	void triggered(QPointF);

private:
	QPointF pos;
};


#endif /* PROCESSSCENE_H_ */
