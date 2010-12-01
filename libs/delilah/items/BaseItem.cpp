/*
 * FILE:		BaseItem.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include <QSize>

#include "BaseItem.h"
#include "WorkspaceScene.h"
#include "globals.h"
#include "ConnectionItem.h"

BaseItem::BaseItem(QSvgRenderer* renderer, QGraphicsItem* parent)
	: QGraphicsSvgItem(parent)
{
	setSharedRenderer(renderer);
	init();
}

BaseItem::~BaseItem()
{}

void BaseItem::init()
{
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
	text_item = 0;
}

void BaseItem::setPos(const QPointF &pos)
{
	QRectF bound_rect(QGraphicsSvgItem::boundingRect());
	QPointF new_pos = pos - mapToScene(bound_rect.center());

	QGraphicsSvgItem::setPos(new_pos);
}

void BaseItem::setPos(qreal x,qreal y)
{
	setPos(QPointF(x, y));
}

void BaseItem::setDefaultSize()
{
	if (!default_size.isValid())
		initializeDefaultSize();

	setSize(default_size);
}

void BaseItem::setSize(QSize size)
{
	QSizeF bound_size = QRectF(QGraphicsSvgItem::boundingRect()).size();
	bound_size.scale(size.width(), size.height(), Qt::KeepAspectRatio);
	qreal scale_factor = bound_size.width() / QGraphicsSvgItem::boundingRect().width();

	prepareGeometryChange();
	setScale(scale_factor);
}

void BaseItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	WorkspaceScene* s = (WorkspaceScene*)scene();
	switch(s->getTool())
	{
		case TOOL_SELECT:
			QGraphicsSvgItem::mousePressEvent(event);
			putOnTop();
			break;
		default:
			break;
	}
}

void BaseItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	WorkspaceScene* s = (WorkspaceScene*)scene();
	switch(s->getTool())
	{
		case TOOL_SELECT:
			QGraphicsSvgItem::mouseMoveEvent(event);
			break;
		default:
			break;
	}
}

void BaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	WorkspaceScene* s = (WorkspaceScene*)scene();
	switch(s->getTool())
	{
		case TOOL_SELECT:
			QGraphicsSvgItem::mouseReleaseEvent(event);
			restoreOrder();
			break;
		default:
			break;
	}

}

QVariant BaseItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemPositionChange)
		emit posChanged();

	return QGraphicsSvgItem::itemChange(change, value);
}

bool BaseItem::isConnected(BaseItem* item)
{
	ConnectionItem* conn;
	foreach (conn, connections)
	{
		if ( item == conn->endItem())
			return true;
	}

	return false;
}
