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

#include <QObject>

#include "QsiAlignment.h"       // Alignment



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
	int      xInitial; // Initial X-position
	int      yInitial; // Initial Y-position
	int      x;        // x offset within the QsiBox
	int      y;        // y offset within the QsiBox
	int      width;    // width  of a QsiBlock or total width  of all components in a QsiBox
	int      height;   // height of a QsiBlock or total height of all components in a QsiBox
	bool     visible;

public:
	char* name;        // all QsiBlocks and QsiBoxes have a name, mostly for debugging purposes
	

	QsiBase(QsiBox* _owner, QsiType _type, const char* _name, int _x, int _y, int _width = -1, int _height = -1);
	~QsiBase();

	virtual void  moveRelative(int x, int y)                                = 0;
	virtual void  moveAbsolute(int x, int y)                                = 0;

	virtual int   geometry(int* xP, int* yP, int* widthP, int* heightP)     = 0;
	int           xGet(void);
	int           yGet(void);

	virtual void  hide(void)                                                = 0;
	virtual void  show(void)                                                = 0;
	virtual bool  isVisible(void)                                           = 0;
	virtual void  align(Alignment::Type type, QsiBase* master, int margin)  = 0;

	QsiBox*       getOwner(void);
	const char*   typeName(void);
};

}

#endif
