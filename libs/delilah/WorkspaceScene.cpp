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
#include "BaseItem.h"
#include "QueueItem.h"
#include "Queue.h"
#include "OperationItem.h"
#include "Operation.h"
#include "ConnectionItem.h"
#include "Process.h"
#include "Misc.h"

QSvgRenderer* WorkspaceScene::queue_renderer = 0;
QSvgRenderer* WorkspaceScene::operation_renderer = 0;

WorkspaceScene::WorkspaceScene(QObject* parent)
	: QGraphicsScene(parent)
{
	if (queue_renderer == 0)
		queue_renderer = new QSvgRenderer(QLatin1String(":/svg/objects/queue_box.svg"));
	if (operation_renderer == 0)
		operation_renderer = new QSvgRenderer(QLatin1String(":/svg/objects/operation_box.svg"));

	current_conn = 0;
}

WorkspaceScene::~WorkspaceScene()
{
	if (current_conn)
		delete current_conn;
}

void WorkspaceScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	QGraphicsScene::contextMenuEvent(event);

	if (!event->isAccepted())
	{
		QMenu* menu = new QMenu();

		ActionWithPos* add_queue_act = new ActionWithPos("Add Queue", this);
		add_queue_act->setPosition(event->scenePos());
		connect(add_queue_act, SIGNAL(triggered(QPointF)), this, SIGNAL(addQueueRequested(QPointF)));

		ActionWithPos* add_operation_act = new ActionWithPos("Add Operation", this);
		add_operation_act->setPosition(event->scenePos());
		connect(add_operation_act, SIGNAL(triggered(QPointF)), this, SIGNAL(addOperationRequested(QPointF)));

		menu->addAction(add_queue_act);
		menu->addAction(add_operation_act);

		menu->addAction("Zoom In", this, SLOT(zoomIn()));
		menu->addAction("Zoom Out", this, SLOT(zoomOut()));
		menu->addAction("Zoom 1:1", this, SLOT(zoomReset()));
		menu->exec(event->screenPos());

		delete add_queue_act;
		delete add_operation_act;
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
		BaseItem* item = findItem(event->scenePos());
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
				emit(addOperationRequested(event->scenePos()));
				break;
			case TOOL_CONNECT:
				if (current_conn)
				{
					BaseItem* item = findItem(event->scenePos());
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

BaseItem* WorkspaceScene::findItem(const QPointF &pos)
{
	BaseItem* item = 0;

	QList<QGraphicsItem*> selected = items(pos, Qt::IntersectsItemShape, Qt::DescendingOrder);

	int i = 0;
	while (i<selected.size() && !item)
	{
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
	connect(queue_item, SIGNAL(infoRequested(BaseObject*)), this, SIGNAL(infoRequested(BaseObject*)));
	connect(queue_item, SIGNAL(removeItemRequested(BaseItem*)), this, SLOT(removeItemFromWorkspace(BaseItem*)));
	connect(queue_item, SIGNAL(deleteQueueRequested(Queue*)), this, SIGNAL(deleteQueueRequested(Queue*)));
	connect(queue_item, SIGNAL(uploadDataRequested(Queue*)), this, SIGNAL(uploadDataRequested(Queue*)));
	connect(queue_item, SIGNAL(downloadDataRequested(Queue*)), this, SIGNAL(downloadDataRequested(Queue*)));

	queue_item->setSharedRenderer(queue_renderer);
	queue_item->initText();
	queue_item->setDefaultSize();
	queue_item->setPos(pos);

	addItem(queue_item);
}

void WorkspaceScene::removeItemFromWorkspace(BaseItem* item)
{
	//TODO: remove connections and remove from process

	removeItem(item);
}

void WorkspaceScene::removeQueueItem(Queue* queue)
{
	QList<QGraphicsItem*> scene_items = items();

	for(int i=0; i<scene_items.size(); i++)
	{
		if (scene_items[i]->type()==QUEUE_ITEM)
		{
			QueueItem* item = qgraphicsitem_cast<QueueItem*>(scene_items[i]);
			if (item->queue == queue)
			{
				removeItemFromWorkspace(item);
				break;
			}
		}
	}
}

void WorkspaceScene::showOperation(Operation* operation, const QPointF &position)
{
	OperationItem* operation_item = new OperationItem(operation);

	// Connect appropriate signals to propagate user's requests
	connect(operation_item, SIGNAL(infoRequested(BaseObject*)), this, SIGNAL(infoRequested(BaseObject*)));
	connect(operation_item, SIGNAL(removeItemRequested(BaseItem*)), this, SLOT(removeItemFromWorkspace(BaseItem*)));

	operation_item->setSharedRenderer(operation_renderer);
	operation_item->initText();
	operation_item->setDefaultSize();
	operation_item->setPos(position);

	addItem(operation_item);

	Process* process = new Process(operation_item);
	processes.append(process);
}

void WorkspaceScene::startConnection(BaseItem* item)
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

void WorkspaceScene::closeConnection(BaseItem* item)
{
	// Cancel connections that are between two queues or two operations
	if(current_conn->startItem()->type()==item->type())
	{
		QString error = QString("Items of the same type can not be connected.");
		emit(unhandledFailure(error));
		cancelConnection();
	}
	else
	{
		OperationItem* operation_item = 0;
		QueueItem* queue_item = 0;
		if(current_conn->startItem()->type()==OPERATION_ITEM && item->type()==QUEUE_ITEM)
		{
			operation_item = qgraphicsitem_cast<OperationItem*>(current_conn->startItem());
			queue_item = qgraphicsitem_cast<QueueItem*>(item);
			Process* process = findProcess(operation_item);
			if (process==0)
			{
				QString error = QString("Process with operation %1 is not available").arg(operation_item->operation->getName());
				emit(unhandledFailure(error));
				cancelConnection();
			}
			else
			{
				QString error = process->addOutput(queue_item);
				if(error.isNull())
				{
					current_conn->close(item);
					current_conn = 0;
				}
				else
				{
					emit(unhandledFailure(error));
					cancelConnection();
				}
			}
		}
		else
		{
			if (item->type()==OPERATION_ITEM && current_conn->startItem()->type()==QUEUE_ITEM)
			{
				operation_item = qgraphicsitem_cast<OperationItem*>(item);
				queue_item = qgraphicsitem_cast<QueueItem*>(current_conn->startItem());

				Process* process = findProcess(operation_item);
				if (process==0)
				{
					QString error = QString("Process with operation %1 is not available").arg(operation_item->operation->getName());
					emit(unhandledFailure(error));
					cancelConnection();
				}
				else
				{
					QString error = process->addInput(queue_item);
					if(error.isNull())
					{
						current_conn->close(item);
						current_conn = 0;
					}
					else
					{
						emit(unhandledFailure(error));
						cancelConnection();
					}
				}
			}
			else
			{
				QString error = QString("Connection is possible only between queue and operation.");
				emit(unhandledFailure(error));
				cancelConnection();
			}
		}
	}
}

Process* WorkspaceScene::findProcess(OperationItem* item)
{
	Process* process = 0;
	for(int i=0; i<processes.size(); i++)
	{
		if (processes[i]->operation_item==item)
		{
			process = processes[i];
			break;
		}
	}

	return process;
}
