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

#include "QsiAlignment.h"       // QsiAlignment
#include "QsiFunction.h"        // QsiFunction
#include "QsiManager.h"         // QsiManager
#include "QsiBase.h"            // QsiBase
#include "QsiBlock.h"           // QsiBlock
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
* QsiBox::QsiBox - 
*/
QsiBox::QsiBox(QsiManager* manager, QsiBox* owner, const char* name, int x, int y) : QsiBase(owner, Box, name, x, y, -1, -1)
{
	qsiVecSize    = QSIS;
	qsiVec        = (QsiBase**) calloc(qsiVecSize, sizeof(QsiBase*));

	alignVecSize  = ALIGNS;
	alignVec      = (Alignment**) calloc(alignVecSize, sizeof(Alignment*)); 

	this->manager = manager;
	type          = Box;

	this->x = xInitial;
	this->y = yInitial;
}



/* ****************************************************************************
*
* moveAbsolute - absolute move of all qsis inside this container
*/
void QsiBox::moveAbsolute(int x, int y)
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
	
	LM_TODO(("Implement BORDER and move it"));

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);
}



/* ****************************************************************************
*
* moveRelative - relative move of all qsis inside this container
*/
void QsiBox::moveRelative(int x, int y)
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

	LM_TODO(("Implement BORDER and move it"));

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);
}



/* ****************************************************************************
*
* geometry - 
*/
int QsiBox::geometry(int* xP, int* yP, int* widthP, int* heightP)
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
void QsiBox::hide(void)
{
	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		qsiVec[ix]->hide();
	}

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);
}



/* ****************************************************************************
*
* show - 
*/
void QsiBox::show(void)
{
	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		qsiVec[ix]->show();
	}

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);
}



/* ****************************************************************************
*
* initialMove - initial positioning of newly created QSI
*/
void QsiBox::initialMove(QsiBase* qbP)
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
void QsiBox::add(QsiBase* qbP)
{
	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] != NULL)
			continue;

		qsiVec[ix] = qbP;

		if (qbP->type != Box)
			initialMove(qbP);

		LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
		sizeChange(this);

		return;
	}

	qsiVec = (QsiBase**) realloc(qsiVec, (qsiVecSize + QSIS) * sizeof(QsiBase*));
    for (int ix = qsiVecSize; ix < qsiVecSize + QSIS; ix++)
		qsiVec[ix] = NULL;

	qsiVecSize = qsiVecSize + QSIS;
	add(qbP);
}



/* ****************************************************************************
*
* remove - 
*/
void QsiBox::remove(QsiBase* qsi)
{
	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] != qsi)
			continue;

		delete qsi;
		qsiVec[ix] = NULL;

		LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
		sizeChange(this);

		return;
	}
}



/* ****************************************************************************
*
* alignLookup - 
*
* NOTE
* Accepting either master-slave or slave-master.
* Thus, mutual alignment is not allowed.
*/
Alignment* QsiBox::alignLookup(QsiBase* master, QsiBase* slave)
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
void QsiBox::align(Alignment::Type type, QsiBase* master, int margin)
{
	owner->align(master, type, this, margin);
}



/* ****************************************************************************
*
* align - 
*/
void QsiBox::align(QsiBase* master, Alignment::Type type, QsiBase* slave, int margin)
{
	if (master->getOwner() != slave->getOwner())
		LM_RVE(("%s %s(owner:%s) and %s %s(owner:%s) cannot be aligned. They don't have the same owner",
				master->typeName(), master->name, master->getOwner()->name, slave->typeName(), slave->name, slave->getOwner()->name));

	if (master->type != slave->type)
		LM_RVE(("%s %s(owner:%s) and %s %s(owner:%s) cannot be aligned. They aren't of the same type",
				master->typeName(), master->name, master->getOwner()->name, slave->typeName(), slave->name, slave->getOwner()->name));

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

	alignP->type   = type;
	alignP->master = master;
	alignP->slave  = slave;
	alignP->margin = margin;
	
	alignShow("Added Alignment");
	realign(master, type, slave, margin);
}



/* ****************************************************************************
*
* unalign - 
*/
void QsiBox::unalign(QsiBase* master)
{
	owner->unalign(master, this);
}



/* ****************************************************************************
*
* unalign - 
*/
void QsiBox::unalign(QsiBase* master, QsiBase* slave)
{
	int unaligns = 0;

	for (int ix = 0; ix < alignVecSize; ix++)
	{
		if (alignVec[ix] == NULL)
			continue;

		if (((alignVec[ix]->master == master) && (alignVec[ix]->slave == slave)) || ((alignVec[ix]->master == slave) && (alignVec[ix]->slave == master)))
		{
			LM_T(LmtAlign, ("Unaligning master '%s' and slave '%s'", master->name, slave->name));

			free(alignVec[ix]);
			alignVec[ix] = NULL;
			++unaligns;
		}
	}

	if (unaligns == 0)
		LM_W(("Cannot unalign '%s' fron '%s' - not found", master->name, slave->name));
}



/* ****************************************************************************
*
* alignShow - 
*/
void QsiBox::alignShow(const char* why)
{
	LM_T(LmtAlignList, (""));
	LM_T(LmtAlignList, ("------------------------ %s: Alignment List (%s) ------------------------", name, why));
	LM_T(LmtAlignList, (""));
	LM_T(LmtAlignList, ("No  %-20s %-20s %-20s  Margin", "Master", "Slave", "Type"));
	LM_T(LmtAlignList, ("--------------------------------------------------------------------------------"));
	for (int ix = 0; ix < alignVecSize; ix++)
	{
		if (alignVec[ix] == NULL)
			continue;

		LM_T(LmtAlignList, ("%02d  %-20s %-20s %-20s  %d", ix, alignVec[ix]->master->name, alignVec[ix]->slave->name, Alignment::name(alignVec[ix]->type), alignVec[ix]->margin));
	}
	LM_T(LmtAlignList, ("--------------------------------------------------------------------------------"));
	LM_T(LmtAlignList, (""));
}



/* ****************************************************************************
*
* realign - 
*/
void QsiBox::realign(QsiBase* master, Alignment::Type type, QsiBase* slave, int margin)
{
	int mx, my, mw, mh;
	int sx, sy, sw, sh;
	int dx, dy;

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

	LM_T(LmtAlign, ("Move slave '%s' %d pixels in X-axis and %d pixels in Y-axis", slave->name, dx, dy));
	slave->moveRelative(dx, dy);

	LM_TODO(("Should make sure 'this' really has changed its geometry before calling sizeChange"));
	sizeChange(this);
}



/* ****************************************************************************
*
* xAbs - 
*/
int QsiBox::xAbs(void)
{
	if (owner == NULL)
		return x;

	return x + owner->xAbs();
}



/* ****************************************************************************
*
* yAbs - 
*/
int QsiBox::yAbs(void)
{
	if (owner == NULL)
		return y;

	return y + owner->yAbs();
}



/* ****************************************************************************
*
* absPos - 
*/
void QsiBox::absPos(int* xP, int* yP)
{
	*xP = xAbs();
	*yP = yAbs();
}



/* ****************************************************************************
*
* sizeChange - 
*/
void QsiBox::sizeChange(QsiBase* qbP)
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
QsiBase* QsiBox::boxAdd(const char* name, int x, int y)
{
	QsiBox* box = new QsiBox(manager, this, name, x, y);

	add(box);
	return box;
}



/* ****************************************************************************
*
* textAdd - 
*/
QsiBase* QsiBox::textAdd(const char* name, const char* txt, int x, int y)
{
	QsiBlock* qbP = new QsiBlock(manager, this, Qsi::SimpleText, name, txt, x, y);

	add(qbP);
	return qbP;
}



/* ****************************************************************************
*
* lineAdd - 
*/
QsiBase* QsiBox::lineAdd(const char* name, int x, int y, int x2, int y2)
{
	QsiBlock* qbP = new QsiBlock(manager, this, Qsi::Line, name, NULL, x, y, x2, y2);

	add(qbP);
	return qbP;
}



/* ****************************************************************************
*
* buttonAdd - 
*/
QsiBase* QsiBox::buttonAdd(const char* name, const char* txt, int x, int y, int width, int height, QsiFunction func, void* param)
{
	QsiBlock* qbP = new QsiBlock(manager, this, Qsi::Button, name, txt, x, y, width, height);

	add(qbP);

	if (func != NULL)
		manager->siConnect(qbP, func, param);

	return qbP;
}



/* ****************************************************************************
*
* inputAdd - 
*/
QsiBase* QsiBox::inputAdd(const char* name, const char* txt, int x, int y, int width, int height)
{
	QsiBlock* qbP = new QsiBlock(manager, this, Qsi::Input, name, txt, x, y, width, height);

	add(qbP);
	return qbP;
}



/* ****************************************************************************
*
* imageAdd - 
*/
QsiBase* QsiBox::imageAdd(const char* name, const char* path, int x, int y, int width, int height, QsiFunction func, void* param)
{
	QsiBlock* qbP = new QsiBlock(manager, this, Qsi::Image, name, path, x, y, width, height);

	add(qbP);

	if (func != NULL)
		manager->siConnect(qbP, func, param);

	return qbP;
}



/* ****************************************************************************
*
* lookup - 
*/
QsiBlock* QsiBox::lookup(QGraphicsItem* gItemP)
{
	QsiBlock* block;

	if (gItemP == NULL)
	{
		LM_T(LmtBlockLookup, ("NULL gItemP"));
		return NULL;
	}

	for (int ix = 0; ix < qsiVecSize; ix++)
	{
		if (qsiVec[ix] == NULL)
			continue;

		if (qsiVec[ix]->type == Box)
		{
			QsiBox* boxP = (QsiBox*) qsiVec[ix];

			LM_T(LmtBlockLookup, ("Entering Box '%s' to lookup %p", boxP->name, gItemP));
			block = boxP->lookup(gItemP);
			if (block != NULL)
				return block;
		}
		else
		{
			QsiBlock* qbP;

			qbP = (QsiBlock*) qsiVec[ix];

			LM_T(LmtBlockLookup, ("Comparing pressed gItem '%p' to %s '%s' gItem '%p'", gItemP, qbP->typeName(), qbP->name, qbP->gItemP));
			if (qbP->gItemP == gItemP)
			{
				LM_T(LmtBlockLookup, ("Found gItem %s '%s' in box '%s'", qbP->typeName(), qbP->name, name));
				return qbP;
			}

			LM_T(LmtBlockLookup, ("Comparing pressed gItem '%p' to %s '%s' proxy '%p'", gItemP, qbP->typeName(), qbP->name, qbP->proxy));
			if (((int) gItemP) == (((int) qbP->proxy) + 8))
			{
				LM_T(LmtBlockLookup, ("Found proxy %s '%s' in box '%s'", qbP->typeName(), qbP->name, name));
				return qbP;
			}
		}
	}

	return NULL;
}

}

