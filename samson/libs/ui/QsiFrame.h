#ifndef QSI_FRAME_H
#define QSI_FRAME_H

/* ****************************************************************************
*
* FILE                     QsiFrame.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 16 2011
*
*/



/* ****************************************************************************
*
* Classes
*/
class QGraphicsLineItem;
class QGraphicsSimpleTextItem;
class Box;



namespace Qsi
{



/* ****************************************************************************
*
* Frame - 
*/
class Frame
{
private:
	int          padding;
	Box*         box;

public:
	Frame(Box* _box, int padding);
	~Frame();

	void paint(void);
	void moveRelative(int x, int y);
	void moveAbsolute(int x, int y);
	void hide(void);
	void show(void);

private:
	QGraphicsLineItem*        north;
	QGraphicsLineItem*        west;
	QGraphicsLineItem*        south;
	QGraphicsLineItem*        east;
};

}

#endif
