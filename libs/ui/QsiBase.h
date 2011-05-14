#ifndef QSI_BASE_H
#define QSI_BASE_H

/* ****************************************************************************
*
* FILE                     QsiBase.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 09 2011
*
*/
#include <stdlib.h>         // free

#include <QObject>



namespace Qsi
{



/* ****************************************************************************
*
* forward declarations
*/
class QsiBox;



typedef enum QsiType
{
	Box,
	SimpleText,
	Image,
	Label,
	Button,
	Input,
	Line
} QsiType;



/* ****************************************************************************
*
* QsiBase - 
*/
class QsiBase
{
	friend class QsiBlock;
	friend class QsiBox;

private:
	QsiBox*  owner;    // the Box this qsi belongs to - used for callback on size changes (only?)
	QsiType  type;     // the type of this QSI
	int      x;        // x offset within the QsiBox
	int      y;        // y offset within the QsiBox
	int      width;    // width  of a QsiBlock or total width  of all components in a QsiBox
	int      height;   // height of a QsiBlock or total height of all components in a QsiBox

public:
	char* name;        // all QsiBlocks and QsiBoxes have a name, mostly for debugging purposes
	
	QsiBase(QsiBox* _owner, QsiType _type, const char* _name, int _x, int _y, int _width = -1, int _height = -1)
	{
		owner  = _owner;
		type   = _type;
		name   = strdup(_name);
		x      = _x;
		y      = _y;
		width  = _width;
		height = _height;
	};

	~QsiBase()
	{
		if (name)
			free(name);
	};

	virtual void  moveRelative(int x, int y)                             = 0;
	virtual void  moveAbsolute(int x, int y)                             = 0;
	virtual void  geometry(int* xP, int* yP, int* widthP, int* heightP)  = 0;
	virtual void  hide(void)                                             = 0;
	virtual void  show(void)                                             = 0;

	QsiBox* getOwner(void)
	{
		return owner;
	}

	const char*   typeName(void)
	{
		switch (type)
		{
		case Box:              return "Box";
		case SimpleText:       return "SimpleText";
		case Line:             return "Line";
		case Image:            return "Image";
		case Label:            return "Label";
		case Button:           return "Button";
		case Input:            return "Input";
		}

		return "Unknown Type";
	}
};

}

#endif
