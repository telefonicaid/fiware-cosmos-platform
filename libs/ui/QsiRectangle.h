#ifndef QSI_RECTANGLE_H
#define QSI_RECTANGLE_H

/* ****************************************************************************
*
* FILE                     QsiRectangle.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 18 2011
*
*/
#include <QColor>

#include "QsiBlock.h"           // Block



namespace Qsi
{



/* ****************************************************************************
*
* Classes
*/
class QGraphicsRectItem;
class QGraphicsItem;
class Box;



/* ****************************************************************************
*
* Rectangle - 
*/
class Rectangle : public Block
{
private:
	QGraphicsRectItem*  rect;

public:
	Rectangle(Box* owner, const char* name, int x, int y, int width, int height, QColor fg, QColor bg, int penWidth);
	~Rectangle();

	QGraphicsItem* gItem(void) { return rect; }
};

}

#endif
