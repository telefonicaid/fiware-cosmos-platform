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
#include "ObjectItem.h"
#include "QueueItem.h"
#include "OperationItem.h"
#include "ConnectionItem.h"

QSvgRenderer* ProcessScene::queue_renderer = 0;
QSvgRenderer* ProcessScene::operation_renderer = 0;

ProcessScene::ProcessScene(QObject* parent)
	: QGraphicsScene(parent)
{
	if (queue_renderer == 0)
		queue_renderer = new QSvgRenderer(QLatin1String(":/svg/objects/queue.svg"));
	if (operation_renderer == 0)
		operation_renderer = new QSvgRenderer(QLatin1String(":/svg/objects/operation.svg"));

	current_conn = 0;
}

ProcessScene::~ProcessScene()
{
	if (current_conn)
		delete current_conn;
}

void ProcessScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	//TODO:
	//TODO: get possible actions (always for global scene, additional for item)
	QGraphicsScene::contextMenuEvent(event);

	if (!event->isAccepted())
	{
		QMenu* menu = new QMenu();

//		menu->addAction("Add Queue", this, SLOT(addQueue()));
		NewQueueAction* act = new NewQueueAction("Add Queue", this);
		act->setPosition(event->scenePos());
		connect(act, SIGNAL(triggered(QPointF)), this, SLOT(addQueue(QPointF)));
		menu->addAction(act);

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

void ProcessScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (current_tool==TOOL_CONNECT && event->button()==Qt::LeftButton)
	{
		std::cout << "Pressed\n";
		ObjectItem* item = findItem(event->scenePos());
		if (item)
			startConnection(item);
	}
	else
		QGraphicsScene::mousePressEvent(event);

}

void ProcessScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (current_tool==TOOL_CONNECT && current_conn)
	{
		current_conn->updateEndPos(event->scenePos());
	}
	else
	{
		QGraphicsScene::mouseMoveEvent(event);
	}
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
				addOperation(event->scenePos());
				break;
			case TOOL_CONNECT:
				if (current_conn)
				{
					std::cout << "Released\n";

					ObjectItem* item = findItem(event->scenePos());
					if (item && item!=current_conn->startItem())
					{
						closeConnection(item);
					}
					else
					{
						cancelConnection();
					}
				}
				break;

			case TOOL_SELECT:
			default:
				break;
		}
}

ObjectItem* ProcessScene::findItem(const QPointF &pos)
{
	ObjectItem* item = 0;

	QList<QGraphicsItem*> selected = items(pos, Qt::IntersectsItemShape, Qt::DescendingOrder);

	int i = 0;
	while (i<selected.size() && !item)
	{
		std::cout << "Analizing type = " << selected[i]->type() << "\n";
		switch(selected[i]->type())
		{
			case QUEUE_ITEM:
				item = qgraphicsitem_cast<QueueItem*>(selected[i]);
				break;
			case OPERATION_ITEM:
				item = qgraphicsitem_cast<OperationItem*>(selected[i]);
				break;
			default:
				item = 0;
				break;
		}
		i++;
	}
	return item;
}

void ProcessScene::addQueue(QPointF position)
{
	QueueItem* queue = new QueueItem();
	queue->setSharedRenderer(queue_renderer);
	queue->initText(QString("Test Queue"));
	queue->setDefaultSize();
	queue->setPos(position);

	addItem(queue);
}

void ProcessScene::addOperation(QPointF position)
{
	OperationItem* operation = new OperationItem();
	operation->setSharedRenderer(operation_renderer);
	operation->setDefaultSize();
	operation->setPos(position);

	addItem(operation);

}

void ProcessScene::startConnection(ObjectItem* item)
{
	if (current_conn)
		delete current_conn;

	current_conn = new ConnectionItem(item);
	addItem(current_conn);
}

void ProcessScene::cancelConnection()
{
	removeItem(current_conn);
	current_conn = 0;
}

void ProcessScene::closeConnection(ObjectItem* item)
{
	if (current_conn->close(item))
	{
		current_conn = 0;
	}
	else
	{
		cancelConnection();
	}
}
