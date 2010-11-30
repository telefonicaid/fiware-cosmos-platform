/*
 * OperationItem.cpp
 *
 *  Created on: Oct 22, 2010
 *      Author: ania
 */

#include <QMenu>
#include <QGraphicsSceneMouseEvent>

#include "OperationItem.h"
#include "Operation.h"

void OperationItem::initializeDefaultSize()
{
	default_size = QSize(112, 121);
}

void OperationItem::initText()
{
	if(operation==0)
		return;

	if (text_item!=0)
		delete text_item;

	QString text = operation->getName();
	text_item = new ObjectTextItem(text, this);
	// Don't know why this dosn't work...
//	text_item->setAcceptedMouseButtons(0);
	QFont serifFont("Times", 12, QFont::Bold);
	text_item->setFont(serifFont);

	QRectF item_rect(boundingRect());
	QRectF text_rect(text_item->boundingRect());

	// scale
	qreal scale_factor = 0.8*item_rect.width()/text_rect.width();
	text_item->setScale(scale_factor);

	// move to center
	QPointF p = item_rect.center() - text_item->mapToParent(text_rect.center());
	text_item->moveBy(p.x(), p.y());
}


void OperationItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu* menu = new QMenu();
	menu->addAction("Show Operation Info", this, SLOT(showInfoSelected()));
	menu->addAction("Remove Operation", this, SLOT(removeOperationSelected()));
	menu->addAction("Run Operation", this, SLOT(runOperationSelected()));
	menu->exec(event->screenPos());
}

void OperationItem::showInfoSelected()
{
	emit(infoRequested(operation));
}

void OperationItem::removeOperationSelected()
{
	// TODO:
}

void OperationItem::runOperationSelected()
{
	// TODO:
}
