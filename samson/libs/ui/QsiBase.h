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



/* ****************************************************************************
*
* MAX - 
*/
#ifndef MAX
#define MAX(a, b)    (((a) > (b))? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b)    (((a) < (b))? (a) : (b))
#endif



namespace Qsi
{



/* ****************************************************************************
*
* forward declarations
*/
class Box;



typedef enum Type
{
	Line,
	Rectangle,
	SimpleText,
	Image,

	Button,
	Input,
	Combo,

	BoxItem,
	ExpandListItem,
	InputLineItem,
	DialogItem,
	InputDialogItem
} Type;



/* ****************************************************************************
*
* Base - 
*/
class Base
{
	friend class Block;
	friend class Box;

public:
	Box*       owner;    // the Box this qsi belongs to - used for callback on size changes (only?)
	Type       type;     // the type of this QSI
	bool       isBox;    // Is it a Box ot not ...

	int        xInitial; // Initial X-position
	int        yInitial; // Initial Y-position
	int        x;        // x offset within the Box
	int        y;        // y offset within the Box
	int        initialMoves;

private:
	int        width;    // width  of a Block or total width  of all components in a Box
	int        height;   // height of a Block or total height of all components in a Box
	bool       visible;

public:
	char*      name;     // all Blocks and Boxes have a name, mostly for debugging purposes
	

	Base(Box* _owner, Type _type, const char* _name, int _x, int _y, int _width = -1, int _height = -1);
	~Base();

	virtual void  moveRelative(int x, int y)                                = 0;
	virtual void  moveAbsolute(int x, int y)                                = 0;

	virtual int   geometry(int* xP, int* yP, int* widthP, int* heightP)     = 0;
	int           xGet(void);
	void          xSet(int _x);
	int           yGet(void);
	void          ySet(int _y);

	virtual void  hide(void)                                                = 0;
	virtual void  show(void)                                                = 0;
	virtual bool  isVisible(void)                                           = 0;
	virtual void  align(Alignment::Type type, Base* master, int margin)     = 0;

	Box*          getOwner(void);
	const char*   typeName(void);
	const char*   typeName(Type type);
	void          typeSet(Type _type);

	bool          isAncestor(Box* qbP);
};

}

#endif
