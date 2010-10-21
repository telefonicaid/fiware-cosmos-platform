/*
 * ProcessScene.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: ania
 */

#include <iostream>

#include <QMenu>
#include <QGraphicsView>
#include <QSvgRenderer>

#include "ProcessScene.h"
#include "QueueItem.h"

QSvgRenderer* ProcessScene::queue_renderer = 0;

ProcessScene::ProcessScene(QObject* parent)
	: QGraphicsScene(parent)
{
	if (queue_renderer == 0)
		queue_renderer = new QSvgRenderer(QLatin1String(":/svg/objects/queue.svg"));
}

ProcessScene::~ProcessScene()
{

}

void ProcessScene::setTool(int tool)
{
	current_tool = tool;
}

int ProcessScene::getTool()
{
	return current_tool;
}

void ProcessScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
	//TODO: get possible actions (always for global scene, additional for item)
	//TODO: add real actions;
	QGraphicsScene::contextMenuEvent(event);
	if (!event->isAccepted())
	{
		QMenu* menu = new QMenu();
		menu->addAction("Add Queue", this, SIGNAL(NewQueueRequested(event->scenePos())));
		menu->addAction("Zoom In", this, SLOT(zoomIn()));
		menu->addAction("Zoom Out", this, SLOT(zoomOut()));
		menu->addAction("Zoom 1:1", this, SLOT(zoomReset()));
		menu->exec(event->screenPos());
	}
}

void ProcessScene::zoomIn()
{

	views()[0]->scale(1.5, 1.5);
}

void ProcessScene::zoomOut()
{
	views()[0]->scale(1/1.5, 1/1.5);
}

void ProcessScene::zoomReset()
{
	views()[0]->resetTransform();
}

void ProcessScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsScene::mouseReleaseEvent(event);
	if (!event->isAccepted())
		switch(current_tool)
		{
			case TOOL_NEWQUEUE:
				addQueue(event->scenePos());
				break;
			case TOOL_NEWOPERATION:
				//TODO:
				break;
			case TOOL_SELECT:
				//TODO:
				break;
			default:
				break;
		}
}

void ProcessScene::addQueue(QPointF position)
{
	QueueItem* queue = new QueueItem();
	queue->setSharedRenderer(queue_renderer);
	queue->initText(QString("Test Queue"));
	queue->scaleToDefaultSize();
	queue->setPos(position);

	addItem(queue);
}
