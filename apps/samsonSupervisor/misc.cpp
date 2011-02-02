#include <QRectF>
#include <QPointF>
#include <QGraphicsItem>

#include "misc.h"              // Own interface



/* ****************************************************************************
*
* centerCoordinates - 
*/
void centerCoordinates(QGraphicsItem* itemP, qreal* x, qreal* y)
{
	QRectF  rect;
	QPointF point;
	qreal   rx;
	qreal   ry;
	qreal   width;
	qreal   height;

	rect = itemP->boundingRect();
	rect.getRect(&rx, &ry, &width, &height);

	point = itemP->scenePos();
	
	*x = point.x() + width/2;
	*y = point.y() + height/2;
}
