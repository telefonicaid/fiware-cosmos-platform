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
	void addQueue(QPointF position);
	void zoomOut();
	void zoomReset();
	void zoomIn();

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

signals:

protected:
	static QSvgRenderer* queue_renderer;
    int current_tool;

};


#endif /* PROCESSSCENE_H_ */
