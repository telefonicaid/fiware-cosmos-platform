/*
 * WorkspaceScene.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: ania
 */

#include <iostream>

#include <QMenu>
#include <QGraphicsView>
#include <QSvgRenderer>

#include "WorkspaceScene.h"
#include "ObjectItem.h"
#include "QueueItem.h"
#include "Queue.h"
#include "OperationItem.h"
#include "ConnectionItem.h"
#include "Misc.h"

QSvgRenderer* WorkspaceScene::queue_renderer = 0;
QSvgRenderer* WorkspaceScene::operation_renderer = 0;

WorkspaceScene::WorkspaceScene(QObject* parent)
	: QGraphicsScene(parent)
{
	if (queue_renderer == 0)
		queue_renderer = new QSvgRenderer(QLatin1String(":/svg/objects/queue_box.svg"));
	if (operation_renderer == 0)
		operation_renderer = new QSvgRenderer(QLatin1String(":/svg/objects/operation.svg"));

	current_conn = 0;
}

WorkspaceScene::~WorkspaceScene()
{
	if (current_conn)
		delete current_conn;
}

void WorkspaceScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	//TODO:
	//TODO: get possible actions (always for global scene, additional for item)
	QGraphicsScene::contextMenuEvent(event);

	if (!event->isAccepted())
	{
		QMenu* menu = new QMenu();

		ActionWithPos* add_act = new ActionWithPos("Add Queue", this);
		add_act->setPosition(event->scenePos());
		connect(add_act, SIGNAL(triggered(QPointF)), this, SIGNAL(addQueueRequested(QPointF)));
		menu->addAction(add_act);

		menu->addAction("Zoom In", this, SLOT(zoomIn()));
		menu->addAction("Zoom Out", this, SLOT(zoomOut()));
		menu->addAction("Zoom 1:1", this, SLOT(zoomReset()));
		menu->exec(event->screenPos());

		delete add_act;
	}
}

void WorkspaceScene::zoomIn()
{

	views()[0]->scale(1.5, 1.5);
}

void WorkspaceScene::zoomOut()
{
	views()[0]->scale(1/1.5, 1/1.5);
}

void WorkspaceScene::zoomReset()
{
	views()[0]->resetTransform();
}

void WorkspaceScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (current_tool==TOOL_CONNECT && event->button()==Qt::LeftButton)
	{
		ObjectItem* item = findItem(event->scenePos());
		if (item)
			startConnection(item);
	}
	else
		QGraphicsScene::mousePressEvent(event);

}

void WorkspaceScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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

void WorkspaceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsScene::mouseReleaseEvent(event);
	if (!event->isAccepted())
		switch(current_tool)
		{
			case TOOL_NEWQUEUE:
				emit(addQueueRequested(event->scenePos()));
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

ObjectItem* WorkspaceScene::findItem(const QPointF &pos)
{
	ObjectItem* item = 0;

	QList<QGraphicsItem*> selected = items(pos, Qt::IntersectsItemShape, Qt::DescendingOrder);

	int i = 0;
	while (i<selected.size() && !item)
	{
		// TODO: remove
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

void WorkspaceScene::showQueue(Queue* queue, const QPointF &pos)
{
	QueueItem* queue_item = new QueueItem(queue);

	// Connect appropriate signals to propagate user's requests
	connect(queue_item, SIGNAL(removeQueueFromWorkspaceRequested(Queue*)),
			this, SIGNAL(removeQueueFromWorkspaceRequested(Queue*)));
	connect(queue_item, SIGNAL(deleteQueueRequested(Queue*)),
			this, SIGNAL(deleteQueueRequested(Queue*)));

	queue_item->setSharedRenderer(queue_renderer);
	queue_item->initText();
	queue_item->setDefaultSize();
	queue_item->setPos(pos);

	addItem(queue_item);
}

void WorkspaceScene::removeQueue(Queue* queue)
{
	QList<QGraphicsItem*> scene_items = items();

	for(int i=0; i<scene_items.size(); i++)
	{
		if (scene_items[i]->type()==QUEUE_ITEM)
		{
			QueueItem* item = qgraphicsitem_cast<QueueItem*>(scene_items[i]);
			if (item->queue == queue)
			{
				removeItem(item);
				break;
			}
		}
	}
}

void WorkspaceScene::addOperation(const QPointF &position)
{
	OperationItem* operation = new OperationItem();
	operation->setSharedRenderer(operation_renderer);
	operation->setDefaultSize();
	operation->setPos(position);

	addItem(operation);

}

void WorkspaceScene::startConnection(ObjectItem* item)
{
	if (current_conn)
		delete current_conn;

	current_conn = new ConnectionItem(item);
	addItem(current_conn);
}

void WorkspaceScene::cancelConnection()
{
	removeItem(current_conn);
	current_conn = 0;
}

void WorkspaceScene::closeConnection(ObjectItem* item)
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
