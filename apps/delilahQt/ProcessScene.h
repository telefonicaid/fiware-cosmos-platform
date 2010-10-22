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

class ProcessScene: public QGraphicsScene
{
	Q_OBJECT

public:
	ProcessScene(QObject* parent = 0);
	~ProcessScene();

	int getTool();

public slots:
	void setTool(int tool);
	void addQueue(QPointF position = QPoint(0.0, 0.0) );
	void zoomOut();
	void zoomReset();
	void zoomIn();

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

protected:
	static QSvgRenderer* queue_renderer;
    int current_tool;
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
