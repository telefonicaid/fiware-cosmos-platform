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
class QsiBox;



namespace Qsi
{



/* ****************************************************************************
*
* Frame - 
*/
class Frame
{
private:
	char*           title;
	int             padding;
	QsiBox*         box;

public:
	Frame(QsiBox* _box, const char* _title, int padding);
	~Frame();

	void paint(void);

private:
	QGraphicsLineItem*        north;
	QGraphicsLineItem*        west;
	QGraphicsLineItem*        south;
	QGraphicsLineItem*        east;
	QGraphicsSimpleTextItem*  titleItem;
};

}

#endif
