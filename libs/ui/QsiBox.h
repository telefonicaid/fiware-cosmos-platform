#ifndef QSI_BOX_H
#define QSI_BOX_H

/* ****************************************************************************
*
* FILE                     QsiBox.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 13 2011
*
*/
#include <QColor>

#include "QsiBase.h"            // Base
#include "QsiFunction.h"        // MenuFunction
#include "QsiAlignment.h"       // Alignment
#include "QsiFrame.h"           // Frame



class QGraphicsItem;



namespace Qsi
{



/* ****************************************************************************
*
* ScrollArea - 
*/
typedef struct ScrollArea
{
	Box*  box;
	int   x;
	int   y;
	int   w;
	int   h;
	int   dy;
} ScrollArea;



class Block;
class Manager;
/* ****************************************************************************
*
* Box - 
*/
class Box : public Base
{
	friend class ExpandList;
	friend class Dialog;

public:
	Manager*     manager;
	Block*       firstLine;
	Box*         lastBox;
	bool         vertical;

private:
	Base**       qsiVec;
	int          qsiVecSize;

	Alignment**  alignVec;
	int          alignVecSize;

	Base*        lastAdded;
	
	int          xAbs(void);
	int          yAbs(void);

public:
	Box(Manager* manager, Box* owner, const char* name, int x, int y);
	~Box();

	void setVertical(bool _vertical);

	Manager*    managerGet(void) { return manager; };
	void        moveRelative(int x, int y);                                                  // move all qsis in qsiVec
	void        moveAbsolute(int x, int y);                                                  // move all qsis in qsiVec

	int         geometry(int* xP, int* yP, int* widthP, int* heightP);                       // return geometry of this Box
	int         width(void);
	int         height(void);

	void        hide(void);                                                                  // make all aligned qsis move
	void        show(void);                                                                  // make all aligned qsis move

	void        initialMove(Base* qbP);                                                      // move child to absolute position + relative box position
	void        absPos(int* xP, int* yP);                                                    // get absolute position - recursively
	void        add(Base* qsi);                                                              // add a qsi (Box or Block)
	void        addVertically(Base* qsi);                                                    // add a qsi (Box or Block)
	Base*       lastAddedGet(void);

	void        remove(Base* qsi, bool destroy = true);                                      // remove a qsi (Box or Block)

	void        align(Alignment::Type type, Base* master, int margin);                       // align this Box to another Box
	void        align(Base* master, Alignment::Type type, Base* slave, int margin);          // align qsis
	void        unalign(int ix);                                                             // remove an identified alignment
	void        unalign(Base* master);                                                       // unalign this Box from another Box
	void        unalign(Base* master, Base* slave);                                          // remove alignment
	void        realign(Base* master, Alignment::Type type, Base* slave, int margin);        // absolute move part of alignment
	void        realign(void);                                                               // after removol, realing all upwards
	void        alignFix(Base* qbP);                                                         // Mend alignments at removal of an item
	Alignment*  alignLookup(Base* master, Base* slave);                                      // wont have mor than alignment between two qsis
	void        alignShow(const char* why, bool force = false);                              // Show list of alignment vector
	
	void        sizeChange(Base* qsi);                                                       // callback - a child has changed its size

	virtual Block*   lookup(QGraphicsItem* gItemP);

	Base*    boxAdd(const char*    name,                      int x, int y);

	Base*    lineAdd(const char*   name,                      int x, int y, int x2, int y2);
	Base*    rectangleAdd(const char*  name,                  int x, int y, int width,      int height, QColor bg, QColor fg, int borderWidth, MenuFunction func = NULL, void* param = NULL);
	Base*    textAdd(const char*   name, const char* txt,     int x, int y);

	Base*    inputAdd(const char*  name, const char* txt,     int x, int y, int width = -1, int height = -1);
	Base*    buttonAdd(const char* name, const char* txt,     int x, int y, int width = -1, int height = -1,                                   MenuFunction func = NULL, void* param = NULL);
	Base*    imageAdd(const char*  name, const char* path,    int x, int y, int width = -1, int height = -1,                                   MenuFunction func = NULL, void* param = NULL);
	Base*    comboAdd(const char*  name, const char** option, int x, int y, int width = -1, int height = -1,                                   MenuFunction func = NULL, void* param = NULL);

	void     qsiShow(const char* why, bool force = false);
	void     qsiRecursiveShow(const char* why, bool force = false);

	bool     isVisible(void) { return true; }

	virtual void  setFrame(int padding);
	Frame*        frame;

	bool  scrollable;
	void  setScrollable(bool _scrollable);
	void  scroll(int dy);

private:
	ScrollArea** scrollVec;
	int          scrollVecSize;
	ScrollArea*  scrollAreaLookup(Box* sbox, int* ixP);

public:
	ScrollArea*  scrollAreaSet(Box* box, int x, int y, int w, int h, int dy, bool on);
	ScrollArea*  scrollAreaLookup(int px, int py);
};

}

#endif
