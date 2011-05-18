/* ****************************************************************************
*
* FILE                     QsiRectangle.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 18 2011
*
*/
#include "QsiBox.h"             // Box



namespace Qsi
{



/* ****************************************************************************
*
* Rectangle - 
*/
Rectangle::Rectangle(Box* owner, const char* name, int x, int y, int width, int height, QColor fg, QColor bg, int penWidth) : 
Block(owner->manager, owner, RectangleItem, name, NULL, x, y, width, height);
{
	QPen    pen(QBrush(fg), penWidth);
	QBrush  brush(bg);

	rect = owner->manager->addRect(x, y, width, height, pen, brush);
	rect->setPos(0, 0);
	owner->add(this);
}



/* ****************************************************************************
*
* ~Rectangle - 
*/
~Rectangle()
{
	delete rect;
}



/* ****************************************************************************
*
* gItem - 
*/
QGraphicsItem* gItem(void)
{
	return rect;
}

}
