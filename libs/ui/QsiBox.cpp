/* ****************************************************************************
*
* FILE                     QsiBox.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 13 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiAlignment.h"       // Alignment
#include "QsiFunction.h"        // Function
#include "QsiManager.h"         // Manager
#include "QsiBase.h"            // Base
#include "QsiBlock.h"           // Block
#include "QsiFrame.h"           // Frame
#include "QsiExpandList.h"      // ExpandList
#include "QsiBox.h"             // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* QSIS   - initial amount and also increment amount for no of qsis in qsiVec
* ALIGN - initial amount and also increment amount for no of aligns in alignVec
*/
#define QSIS    5
#define ALIGNS  3



/* ****************************************************************************
*
* MAX - 
*/
#define MAX(a, b)    (((a) > (b))? (a) : (b))
#define MIN(a, b)    (((a) < (b))? (a) : (b))



/* ****************************************************************************
*
* Box::Box - 
*/
Box::Box(Manager* manager, Box* owner, const char* name, int x, int y) : Base(owner, BoxItem, name, x, y, -1, -1)
{
	qsiVecSize    = QSIS;
	qsiVec        = (Base**) calloc(qsiVecSize, sizeof(Base*));

	alignVecSize  = ALIGNS;
	alignVec      = (Alignment**) calloc(alignVecSize, sizeof(Alignment*)); 

	this->manager = manager;
	type          = BoxItem;

	this->x = xInitial;
	this->y = yInitial;

	frame     = NULL;
	lastAdded = NULL;
	isBox     = true;

	firstLine = (Block*) lineAdd("firstLine", 0, 0, 10, 0);
	firstLine->setColor(0xFF, 0xFF, 0xFF, 0);

}


/* ****************************************************************************
*
* setFrame - 
*/
void Box::setFrame(int padding)
{
	if (padding == -19)
	{
		LM_W(("deleting frame"));

		if (frame != NULL)
			delete frame;
		frame = NULL;
	}
	else
	{
		LM_T(LmtFrame, ("Creating Frame"));
		frame = new Frame(this, padding);

		int x, y;
		absPos(&x, &y);
		frame->moveAbsolute(x - padding - 10, y - padding + 8);
	}
}



/* ****************************************************************************
*
* moveAbsolute - absolute move of all qsis inside this container
*/
void Box::moveAbsolute(int x, int y)
{
	this->x = x;
	this->y = y;

	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		LM_T(LmtAbsMove, ("  Moving %s '%s' to absolute position { %d, %d }", qsiVec[ix]->typeName(), qsiVec[ix]->name, this->x, this->y));
		qsiVec[ix]->moveAbsolute(this->x, this->y);
		
		LM_T(LmtAbsMove, ("  Moving %s '%s' to relative position { %d, %d }", qsiVec[ix]->typeName(), qsiVec[ix]->name, qsiVec[ix]->x, qsiVec[ix]->y));
		qsiVec[ix]->moveRelative(qsiVec[ix]->x, qsiVec[ix]->y);
	}
	
	if (frame)
		frame->moveAbsolute(x, y);

	LM_TODO(("Implement BORDER and move it"));

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);

	if (frame != NULL)
		LM_TODO(("Move the Frame too ..."));
}



/* ****************************************************************************
*
* moveRelative - relative move of all qsis inside this container
*/
void Box::moveRelative(int x, int y)
{
	if (owner == NULL)
		LM_RVE(("Not moving Toplevel Box!"));

	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		qsiVec[ix]->moveRelative(x, y);
	}
	
	this->x += x;
	this->y += y;

	if (frame)
		frame->moveRelative(x, y);

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);

	if (frame != NULL)
		LM_TODO(("Move the Frame too ..."));
}



/* ****************************************************************************
*
* geometry - 
*/
int Box::geometry(int* xP, int* yP, int* widthP, int* heightP)
{
	int xMax = -500000;
	int yMax = -500000;
	int xMin = 0x7FFFFFFF;
	int yMin = 0x7FFFFFFF;

	*xP = x;
	*yP = y;

	LM_T(LmtGeometry, (""));
	LM_T(LmtGeometry, ("Box '%s' geometry:", name));
	LM_T(LmtGeometry, ("----------------------------------"));
	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		int qx, qy, qw, qh;
		
		if (qsiVec[ix] == NULL)
		{
			LM_T(LmtGeometry, ("%02d: NULL", ix));
			continue;
		}

		LM_T(LmtGeometry, ("Checking whether '%s' is visible", qsiVec[ix]->name));
		if (qsiVec[ix]->isVisible() == false)
		{
			LM_T(LmtGeometry, ("%02d: %s '%s' is NOT visible", ix, qsiVec[ix]->typeName(), qsiVec[ix]->name));
			continue;
		}

		qsiVec[ix]->geometry(&qx, &qy, &qw, &qh);
		LM_T(LmtGeometry, ("Geometry for %s '%s': { %d, %d } %d x %d", qsiVec[ix]->typeName(), qsiVec[ix]->name, qx, qy, qw, qh));
		xMax = MAX(xMax, qx + qw);
		yMax = MAX(yMax, qy + qh);
		xMin = MIN(xMin, qx);
		yMin = MIN(yMin, qy);

		LM_T(LmtGeometry, ("xMin = %d, xMax = %d     yMin = %d, yMax = %d         dx = %d, dy = %d", xMin, xMax, yMin, yMax, xMax - xMin, yMax - yMin));
	}

	if ((xMin == 0x7FFFFFFF) || (yMin == 0x7FFFFFFF) || (xMax == -500000) || (yMax == -500000))
	{
		*widthP  = 0;
		*heightP = 0;
		LM_RE(-1, ("No geometry found"));
	}

	*widthP  = xMax - xMin;
	*heightP = yMax - yMin;

	LM_T(LmtGeometry, (""));
	LM_T(LmtGeometry, ("Box '%s' geometry: { %d, %d } %d x %d", name, *xP, *yP, *widthP, *heightP));
	LM_T(LmtGeometry, (""));
	LM_T(LmtGeometry, ("----------------------------------"));
	LM_T(LmtGeometry, (""));

	return 0;
}



/* ****************************************************************************
*
* hide - 
*/
void Box::hide(void)
{
	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		qsiVec[ix]->hide();
	}

	if (frame)
		frame->hide();

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);
}



/* ****************************************************************************
*
* show - 
*/
void Box::show(void)
{
	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		qsiVec[ix]->show();
	}

	if (frame)
		frame->show();

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);
}



/* ****************************************************************************
*
* initialMove - initial positioning of newly created QSI
*/
void Box::initialMove(Base* qbP)
{
	int ax, ay;
	
	absPos(&ax, &ay);

	qbP->moveAbsolute(ax, ay);
	qbP->moveRelative(qbP->xInitial, qbP->yInitial);
}



/* ****************************************************************************
*
* add - 
*/
void Box::add(Base* qbP)
{
	if (qbP->type == ExpandListItem)
		LM_T(LmtExpandList, ("Adding ExpandListItem"));

	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] != NULL)
			continue;

		qsiVec[ix] = qbP;

		if ((qbP->type != BoxItem) && (qbP->type != ExpandListItem))
			initialMove(qbP);

		LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
		sizeChange(this);

		lastAdded = qbP;
		return;
	}

	qsiVec = (Base**) realloc(qsiVec, (qsiVecSize + QSIS) * sizeof(Base*));
    for (int ix = qsiVecSize; ix < qsiVecSize + QSIS; ix++)
		qsiVec[ix] = NULL;

	qsiVecSize = qsiVecSize + QSIS;
	add(qbP);
}



/* ****************************************************************************
*
* remove - 
*/
void Box::remove(Base* qbP)
{
	LM_T(LmtRemove, ("Removing %s '%s'", qbP->typeName(), qbP->name));

	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		LM_T(LmtRemove, ("ix == %d", ix));
		if (qsiVec[ix] != qbP)
			continue;

		qbP->hide();

		LM_T(LmtRemove, ("deleting  %s '%s'", qbP->typeName(), qbP->name));
		alignFix(qbP);

		if (qbP->type == BoxItem)
			delete (Box*) qbP;
		else if (qbP->type == ExpandListItem)
			delete (ExpandList*) qbP;
		else
			delete (Block*) qbP;

		qsiVec[ix] = NULL;

		LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
		owner->sizeChange(owner);

		return;
	}
}



/* ****************************************************************************
*
* alignFix - 
*/
void Box::alignFix(Base* qbP)
{
	Base*            newMaster = NULL;
	Alignment::Type  type      = Alignment::Unaligned;
	int              margin    = 9;

	alignShow("Before removal");
	LM_T(LmtAlign, ("Fixing alignment at removing %s '%s'", qbP->typeName(), qbP->name));



	//
	// 1. What item is the item to be removed aligned to?
	//    I need this to inherit items that are aligned to the item to remove
	//
	for (int ix = 0; ix < alignVecSize; ix++)
	{
		if (alignVec[ix] == NULL)
			continue;
		if (alignVec[ix]->slave == qbP)
		{
			newMaster = alignVec[ix]->master;
			type      = alignVec[ix]->type;
			margin    = alignVec[ix]->margin;

			unalign(alignVec[ix]->master, alignVec[ix]->slave);
			break;
		}
	}



	//
	// 2. lookup all items aligned to 'qbP' and change them so they're aligned to 'newMaster' instead
	//    if 'newMaster' is NULL, just remove the alignment.
	//    Likewise for alignments where 'qbP' is slave, just remove ...
	//
	for (int ix = 0; ix < alignVecSize; ix++)
	{
		if (alignVec[ix] == NULL)
			continue;

		if (alignVec[ix]->master == qbP)
		{
			if (newMaster != NULL)
			{
				LM_W(("%s '%s' is aligned to item-to-be-removed (%s '%s') - aligning him to inheriting master '%s'",
					  alignVec[ix]->slave->typeName(), alignVec[ix]->slave->name, alignVec[ix]->master->typeName(), qbP->name, newMaster->name));

				alignVec[ix]->slave->align(type, newMaster, margin);
				// sizeChange(alignVec[ix]->slave);
			}

			LM_T(LmtAlignVector, ("Removing master-alignment for to-be-removed %s '%s'", qbP->typeName(), qbP->name));
			unalign(ix);
		}
		else if (alignVec[ix]->slave == qbP)
		{
			LM_T(LmtAlignVector, ("Removing slave-alignment for to-be-removed %s '%s'", qbP->typeName(), qbP->name));
			unalign(ix);
		}
	}

	alignShow("After removal");
	realign();
	alignShow("After realignment");
}



/* ****************************************************************************
*
* alignLookup - 
*
* NOTE
* Accepting either master-slave or slave-master.
* Thus, mutual alignment is not allowed.
*/
Alignment* Box::alignLookup(Base* master, Base* slave)
{
	for (int ix = 0; ix < alignVecSize; ix++)
	{
		if (alignVec[ix] == NULL)
			continue;

		if ((alignVec[ix]->master == master) && (alignVec[ix]->slave == slave))
		{
			LM_T(LmtAlignVector, ("Found master '%s' and slave '%s' in alignment vector", master->name, slave->name));
			return alignVec[ix];
		}

		if ((alignVec[ix]->master == slave) && (alignVec[ix]->slave == master))
		{
			LM_T(LmtAlignVector, ("Found master '%s' (as slave) and slave '%s' (as master) in alignment vector", master->name, slave->name));
			return alignVec[ix];
		}
	}

	return NULL;
}



/* ****************************************************************************
*
* align - 
*/
void Box::align(Alignment::Type type, Base* master, int margin)
{
	owner->align(master, type, this, margin);
}



/* ****************************************************************************
*
* align - 
*/
void Box::align(Base* master, Alignment::Type type, Base* slave, int margin)
{
	if (master == NULL)
		master = firstLine;

	if (master->getOwner() != slave->getOwner())
		LM_RVE(("%s %s(owner:%s) and %s %s(owner:%s) cannot be aligned. They don't have the same owner",
				master->typeName(), master->name, master->getOwner()->name, slave->typeName(), slave->name, slave->getOwner()->name));
#if 0
	if (master->type != slave->type)
		LM_RVE(("%s %s(owner:%s) and %s %s(owner:%s) cannot be aligned. They aren't of the same type",
				master->typeName(), master->name, master->getOwner()->name, slave->typeName(), slave->name, slave->getOwner()->name));
#endif

	Alignment* alignP = alignLookup(master, slave);
	
	LM_T(LmtAlign, ("aligning slave %s '%s' to master %s '%s'. Align type: '%s', margin %d", slave->typeName(), slave->name, master->typeName(), master->name, Alignment::name(type), margin));
	if (alignP == NULL)
	{
		for (int ix = 0; ix < alignVecSize; ix++)
		{
			if (alignVec[ix] != NULL)
				continue;

			alignVec[ix] = (Alignment*) calloc(1, sizeof(Alignment));
			alignP       = alignVec[ix];

			break;
		}

		if (alignP == NULL)
		{
			LM_T(LmtAlignVector, ("Reallocating Alignment Vector, adding %d slots", ALIGNS));
			alignVec = (Alignment**) realloc(alignVec, (alignVecSize + ALIGNS) * sizeof(Alignment*));
			for (int ix = alignVecSize; ix < alignVecSize + ALIGNS; ix++)
				alignVec[ix] = NULL;

			alignVecSize = alignVecSize + ALIGNS;
			
			align(master, type, slave, margin);
			return;
		}
	}
	else
		LM_T(LmtAlignVector, ("%s and %s were already aligned - changing their alignment", master->name, slave->name));

	alignP->master = master;
	alignP->slave  = slave;
	alignP->type   = type;
	alignP->margin = margin;
	
	alignShow("Added Alignment");
	realign(master, type, slave, margin);
}



/* ****************************************************************************
*
* unalign - 
*/
void Box::unalign(int ix)
{
	// sizeChange(alignVec[ix]->master);
	// sizeChange(alignVec[ix]->slave);
	free(alignVec[ix]);
	alignVec[ix] = NULL;
}



/* ****************************************************************************
*
* unalign - 
*/
void Box::unalign(Base* master)
{
	owner->unalign(master, this);
}



/* ****************************************************************************
*
* unalign - 
*/
void Box::unalign(Base* master, Base* slave)
{
	int unaligns = 0;

	for (int ix = 0; ix < alignVecSize; ix++)
	{
		if (alignVec[ix] == NULL)
			continue;

		if (((alignVec[ix]->master == master) && (alignVec[ix]->slave == slave)) || ((alignVec[ix]->master == slave) && (alignVec[ix]->slave == master)))
		{
			LM_T(LmtAlign, ("Unaligning master '%s' and slave '%s'", master->name, slave->name));
			unalign(ix);
			++unaligns;
		}
	}

	if (unaligns == 0)
		LM_W(("Cannot unalign '%s' from '%s' - not found", master->name, slave->name));
}



/* ****************************************************************************
*
* alignShow - 
*/
void Box::alignShow(const char* why, bool force)
{
	if (force == true)
	{
		LM_F((""));
		LM_F(("------------------------ %s: Alignment List (%s) ------------------------", name, why));
		LM_F((""));
		LM_F(("No  %-30s %-30s %-20s  Margin", "Master", "Slave", "Type"));
		LM_F(("----------------------------------------------------------------------------------------------------"));
		for (int ix = 0; ix < alignVecSize; ix++)
		{
			if (alignVec[ix] == NULL)
				continue;

			LM_F(("%02d  %-30s %-30s %-20s  %d", ix, alignVec[ix]->master->name, alignVec[ix]->slave->name, Alignment::name(alignVec[ix]->type), alignVec[ix]->margin));
		}
		LM_F(("----------------------------------------------------------------------------------------------------"));
		LM_F((""));
	}
	else
	{
		LM_T(LmtAlignList, (""));
		LM_T(LmtAlignList, ("------------------------ %s: Alignment List (%s) ------------------------", name, why));
		LM_T(LmtAlignList, (""));
		LM_T(LmtAlignList, ("No  %-30s %-30s %-20s  Margin", "Master", "Slave", "Type"));
		LM_T(LmtAlignList, ("----------------------------------------------------------------------------------------------------"));
		for (int ix = 0; ix < alignVecSize; ix++)
		{
			if (alignVec[ix] == NULL)
				continue;

			LM_T(LmtAlignList, ("%02d  %-30s %-30s %-20s  %d", ix, alignVec[ix]->master->name, alignVec[ix]->slave->name, Alignment::name(alignVec[ix]->type), alignVec[ix]->margin));
		}
		LM_T(LmtAlignList, ("----------------------------------------------------------------------------------------------------"));
		LM_T(LmtAlignList, (""));
	}
}



/* ****************************************************************************
*
* realign - 
*/
void Box::realign(void)
{
	for (int ix = 0; ix < alignVecSize; ix++)
	{
		if (alignVec[ix] == NULL)
			continue;

		LM_W(("Realigning slave '%s' to '%s'", alignVec[ix]->slave->name, alignVec[ix]->master->name));
		align(alignVec[ix]->master, alignVec[ix]->type, alignVec[ix]->slave, alignVec[ix]->margin);
	}

	if (owner != NULL)
		owner->realign();
}



/* ****************************************************************************
*
* realign - 
*/
void Box::realign(Base* master, Alignment::Type type, Base* slave, int margin)
{
	int mx, my, mw, mh;
	int sx, sy, sw, sh;
	int dx = 0, dy = 0;

	LM_T(LmtAlign, ("Aligning %s slave '%s' to %s master '%s'. Alignment type: %s, margin: %d", slave->typeName(), slave->name, master->typeName(), master->name, Alignment::name(type), margin));
	master->geometry(&mx, &my, &mw, &mh);
	slave->geometry(&sx, &sy, &sw, &sh);

	LM_T(LmtAlign, ("Master geometry: { %d, %d } %d x %d", mx, my, mw, mh));
	LM_T(LmtAlign, ("Slave  geometry: { %d, %d } %d x %d", sx, sy, sw, sh));

	if (type == Alignment::South)
	{
		dx = mx - sx;
		dy = my - sy + mh + margin;
	}
	else if (type == Alignment::North)
	{
		dx = mx - sx;
		dy = my - sy - sh - margin;
	}
	else if (type == Alignment::East)
	{
		dx = mx - sx + mw + margin;
		dy = my - sy;
	}
	else if (type == Alignment::West)
	{
		dx = mx - sx - sw - margin;
		dy = my - sy;
	}
	else if (type == Alignment::Center)
	{
		dx = mx - sx + (mw - sw) / 2;
		dy = my - sy + (mh - sh) / 2;
	}
	else
		LM_X(1, ("unknown alignment type %d", type));

	LM_T(LmtAlign, ("Move slave '%s' %d pixels in X-axis and %d pixels in Y-axis", slave->name, dx, dy));
	slave->moveRelative(dx, dy);

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);
}



/* ****************************************************************************
*
* xAbs - 
*/
int Box::xAbs(void)
{
	if (owner == NULL)
		return x;

	return x + owner->xAbs();
}



/* ****************************************************************************
*
* yAbs - 
*/
int Box::yAbs(void)
{
	if (owner == NULL)
		return y;

	return y + owner->yAbs();
}



/* ****************************************************************************
*
* absPos - 
*/
void Box::absPos(int* xP, int* yP)
{
	*xP = xAbs();
	*yP = yAbs();
}



/* ****************************************************************************
*
* sizeChange - 
*/
void Box::sizeChange(Base* qbP)
{
	LM_T(LmtSizeChange, ("Size changed for %s '%s'", qbP->typeName(), qbP->name));

	alignShow("Size Change");
	for (int ix = 0; ix < alignVecSize; ix++)
	{
		if (alignVec[ix] == NULL)
			continue;

		if (alignVec[ix]->master == qbP)
		{
			LM_T(LmtSizeChange, ("realigning slave %s '%s' to master '%s'", alignVec[ix]->slave->typeName(), alignVec[ix]->slave->name, alignVec[ix]->master->name));
			realign(alignVec[ix]->master, alignVec[ix]->type, alignVec[ix]->slave, alignVec[ix]->margin);
		}
		else
			LM_T(LmtSizeChange, ("NOT realigning slave %s '%s' to master '%s'", alignVec[ix]->slave->typeName(), alignVec[ix]->slave->name, alignVec[ix]->master->name));
	}	

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling owner's sizeChange"));
	if (owner != NULL)
		owner->sizeChange(this);
}



/* ****************************************************************************
*
* boxAdd - 
*/
Base* Box::boxAdd(const char* name, int x, int y)
{
	Box* box = new Box(manager, this, name, x, y);

	add(box);
	return box;
}



/* ****************************************************************************
*
* textAdd - 
*/
Base* Box::textAdd(const char* name, const char* txt, int x, int y)
{
	Block* qbP = new Block(manager, this, SimpleText, name, txt, x, y);

	add(qbP);
	return qbP;
}



/* ****************************************************************************
*
* lineAdd - 
*/
Base* Box::lineAdd(const char* name, int x, int y, int x2, int y2)
{
	Block* qbP = new Block(manager, this, Line, name, NULL, x, y, x2, y2);

	add(qbP);
	return qbP;
}



/* ****************************************************************************
*
* buttonAdd - 
*/
Base* Box::buttonAdd(const char* name, const char* txt, int x, int y, int width, int height, Function func, void* param)
{
	Block* qbP = new Block(manager, this, Button, name, txt, x, y, width, height);

	add(qbP);

	if (func != NULL)
		manager->siConnect(qbP, func, param);

	return qbP;
}



/* ****************************************************************************
*
* inputAdd - 
*/
Base* Box::inputAdd(const char* name, const char* txt, int x, int y, int width, int height)
{
	Block* qbP = new Block(manager, this, Input, name, txt, x, y, width, height);

	add(qbP);
	return qbP;
}



/* ****************************************************************************
*
* imageAdd - 
*/
Base* Box::imageAdd(const char* name, const char* path, int x, int y, int width, int height, Function func, void* param)
{
	Block* qbP = new Block(manager, this, Image, name, path, x, y, width, height);

	add(qbP);

	if (func != NULL)
		manager->siConnect(qbP, func, param);

	return qbP;
}



/* ****************************************************************************
*
* rectangleAdd - 
*/
Base* Box::rectangleAdd(const char*  name, int x, int y, int width, int height, QColor bg, QColor fg, int borderWidth, Function func, void* param)
{
	QGraphicsRectItem*  rect;
	Block*              qbP   = new Block(manager, this, Rectangle, name, NULL, x, y, width, height);
	QPen                pen(QBrush(fg), borderWidth);
	QBrush              brush(bg);

	if (func != NULL)
		manager->siConnect(qbP, func, param);

	rect = (QGraphicsRectItem*) qbP->gItemP;
	rect->setPen(pen);
	rect->setBrush(brush);

	add(qbP);

	return qbP;
}



/* ****************************************************************************
*
* lookup - 
*/
Block* Box::lookup(QGraphicsItem* gItemP)
{
	Block* block;

	if (gItemP == NULL)
	{
		LM_T(LmtBlockLookup, ("NULL gItemP"));
		return NULL;
	}

	LM_T(LmtBlockLookup, ("Entered Box '%s'", name));
	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		if ((qsiVec[ix]->type == BoxItem) || (qsiVec[ix]->type == ExpandListItem))
		{
			Box* boxP = (Box*) qsiVec[ix];

			LM_T(LmtBlockLookup, ("Entering Box '%s' to lookup %p", boxP->name, gItemP));
			block = boxP->lookup(gItemP);
			if (block != NULL)
				return block;
		}
		else
		{
			Block* qbP;

			qbP = (Block*) qsiVec[ix];

			LM_T(LmtBlockLookup, ("Comparing pressed gItem '%p' to %s '%s' gItem '%p'", gItemP, qbP->typeName(), qbP->name, qbP->gItemP));
			if (qbP->gItemP == gItemP)
			{
				LM_T(LmtBlockLookup, ("Found gItem %s '%s' in box '%s'", qbP->typeName(), qbP->name, name));
				return qbP;
			}

			LM_T(LmtBlockLookup, ("Comparing pressed gItem '%p' to %s '%s' proxy '%p'", gItemP, qbP->typeName(), qbP->name, qbP->proxy));
			if (gItemP == ((QGraphicsItem*) qbP->proxy))
//			if (((long) gItemP) == (((long) qbP->proxy + 16))) // For 64 bit machines
//			if (((long) gItemP) == (((long) qbP->proxy + 8)))  // For 32 bit machines
			{
				LM_T(LmtBlockLookup, ("Found proxy %s '%s' in box '%s'", qbP->typeName(), qbP->name, name));
				return qbP;
			}
		}
	}

	return NULL;
}



/* ****************************************************************************
*
* qsiShow - 
*/
void Box::qsiShow(const char* why, bool force)
{
	if (force == true)
	{
		LM_F((""));
		LM_F(("------------------------ %s: Box Content (%s) ------------------------", name, why));
		LM_F((""));
		LM_F(("No  %-20s %-20s %-5s %-5s  %-20s", "Name", "Type", "x", "y", "QGraphicsItem"));
		LM_F(("----------------------------------------------------------------------------------------------------"));
		for (int ix = 0; ix < qsiVecSize; ix++)
		{
			if (qsiVec[ix] == NULL)
				continue;

			Block* block = (Block*) qsiVec[ix];
			if (qsiVec[ix]->type == BoxItem)
				LM_F(("%02d  %-20s %-20s %-5d %-5d", ix, qsiVec[ix]->name, qsiVec[ix]->typeName(), qsiVec[ix]->xGet(), qsiVec[ix]->yGet()));
			else
				LM_F(("%02d  %-20s %-20s %-5d %-5d  %p", ix, qsiVec[ix]->name, qsiVec[ix]->typeName(), qsiVec[ix]->xGet(), qsiVec[ix]->yGet(), (block->proxy != NULL)? block->proxy : block->gItemP));
		}
		LM_F(("----------------------------------------------------------------------------------------------------"));
		LM_F((""));
	}
	else
	{
		LM_T(LmtQsiList, (""));
		LM_T(LmtQsiList, ("------------------------ %s: Box Content (%s) ------------------------", name, why));
		LM_T(LmtQsiList, (""));
		LM_T(LmtQsiList, ("No  %-20s %-20s %-5d %-5d  %-20s", "Name", "Type", "x", "y", "QGraphicsItem"));
		LM_T(LmtQsiList, ("----------------------------------------------------------------------------------------------------"));
		for (int ix = 0; ix < qsiVecSize; ix++)
		{
			if (qsiVec[ix] == NULL)
				continue;

            Block* block = (Block*) qsiVec[ix];
            if (qsiVec[ix]->type == BoxItem)
				LM_T(LmtQsiList, ("%02d  %-20s %-20s %-5d %-5d", ix, qsiVec[ix]->name, qsiVec[ix]->typeName(), qsiVec[ix]->xGet(), qsiVec[ix]->yGet()));
			else
				LM_T(LmtQsiList, ("%02d  %-20s %-20s %-5d %-5d  %p", ix, qsiVec[ix]->name, qsiVec[ix]->typeName(), qsiVec[ix]->xGet(), qsiVec[ix]->yGet(), (block->proxy != NULL)? block->proxy : block->gItemP));
		}
		LM_T(LmtQsiList, ("----------------------------------------------------------------------------------------------------"));
		LM_T(LmtQsiList, (""));
	}
}


/* ****************************************************************************
*
* qsiRecursiveShow - 
*/
void Box::qsiRecursiveShow(const char* why, bool force)
{
	qsiShow(why, force);

	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		if (qsiVec[ix]->isBox == true)
		{
			Box* box = (Box*) qsiVec[ix];
			box->qsiRecursiveShow(why, force);
		}
	}
}



/* ****************************************************************************
*
* lastAddedGet - 
*/
Base* Box::lastAddedGet(void)
{
	return lastAdded;
}

}
