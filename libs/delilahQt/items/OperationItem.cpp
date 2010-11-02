/*
 * OperationItem.cpp
 *
 *  Created on: Oct 22, 2010
 *      Author: ania
 */

#include <QMenu>
#include <QGraphicsSceneMouseEvent>

#include "OperationItem.h"

void OperationItem::initializeDefaultSize()
{
	// TODO:
	default_size = QSize(80, 40);
}

void OperationItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	// TODO:
	QMenu* menu = new QMenu();
	menu->addAction("There will be some options....");
	menu->exec(event->screenPos());
}
