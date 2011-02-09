/* ****************************************************************************
*
* FILE                     DelilahSceneItem.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <stdio.h>             // snprintf

#include <QPixmap>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>
#include <QMenu>
#include <QAction>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // connectionMgr, ...
#include "misc.h"               // centerCoordinates
#include "DelilahScene.h"       // DelilahScene
#include "DelilahConnection.h"  // DelilahConnection
#include "DelilahSceneItem.h"   // Own interface



/* ****************************************************************************
*
* qNo -
* rNo -
* sNo -
*/
static int qNo = 1;
static int rNo = 1;
static int sNo = 1;



/* ****************************************************************************
*
* DelilahSceneItem::DelilahSceneItem - 
*/
DelilahSceneItem::~DelilahSceneItem()
{
	if (displayName)     delete displayName;
	if (name)            delete name;
	if (pixmap)          delete pixmap;
	if (nameItem)        delete nameItem;
	if (pixmapItem)      delete pixmapItem;

	if (scene->highestInStack == this)
		scene->highestInStack = NULL;
}



/* ****************************************************************************
*
* DelilahSceneItem::DelilahSceneItem - 
*/
DelilahSceneItem::DelilahSceneItem(Type type, DelilahScene* sceneP, const char* imagePath, const char* displayNameP, int x, int y)
{
	char itemName[32];

	disabled      = false;

	inTypeIndex   = 0;
	inType        = strdup("Undefined");

	outTypeIndex  = 0;
	outType       = strdup("Undefined");
	
	this->type = type;

	if (type == Source)
		snprintf(itemName, sizeof(itemName), "Source %d", sNo++);
	else if (type == Queue)
		snprintf(itemName, sizeof(itemName), "Queue %d", qNo++);
	else if (type == Result)
		snprintf(itemName, sizeof(itemName), "Result %d", rNo++);
	else
		LM_X(1, ("Bad type: %d", type));

	if (displayNameP == NULL)
		displayNameP = itemName;

	scene         = sceneP;
	name          = strdup(itemName);
	displayName   = strdup(displayNameP);
	pixmap        = new QPixmap(imagePath);

	xpos          = x;
	ypos          = y;

	pixmapItem    = scene->addPixmap(*pixmap);
	nameItem      = scene->addSimpleText(QString(displayName));
	
	LM_T(LmtMove, ("Calling moveTo for '%s'", displayName));

	moveTo(x, y);
	nameCenter();

	if (scene->highestInStack)
	{
		qreal z;

		z = scene->highestInStack->pixmapItem->zValue();
		z += 0.01;
		pixmapItem->setZValue(z);
		nameItem->setZValue(z);
		
		nameItem->update();
		pixmapItem->update();

		scene->highestInStack->pixmapItem->update();
		scene->highestInStack->nameItem->update();
	}

	scene->highestInStack = this;

	if (tabManager->configTab->autoConfigCBox->checkState() == Qt::Checked)
		scene->config(this);
}



/* ****************************************************************************
*
* moveTo - 
*/
void DelilahSceneItem::moveTo(int x, int y, bool firstTime)
{
	xpos += x;
	ypos += y;

	LM_T(LmtMove, ("********** Moving '%s' to  %d, %d", displayName, xpos, ypos));

	pixmapItem->moveBy(x, y);

	if (firstTime)
		nameItem->moveBy(x, y);

	connectionMgr->move(this);
}



/* ****************************************************************************
*
* DelilahSceneItem::displayNameSet - 
*/
void DelilahSceneItem::displayNameSet(const char* newName)
{
	if (displayName)
		delete displayName;

	displayName = strdup(newName);
	nameItem->setText(QString(displayName));

	nameCenter();
}



/* ****************************************************************************
*
* DelilahSceneItem::nameCenter - 
*/
void DelilahSceneItem::nameCenter(void)
{
	int      xdiff;
	qreal    queueWidth;
	qreal    nameWidth;
	QRectF   rect;
	QPointF  point;
	qreal    rx;
	qreal    ry;
	qreal    height;

	rect = pixmapItem->boundingRect();
	rect.getRect(&rx, &ry, &queueWidth, &height);
	point = pixmapItem->scenePos();

	nameItem->setPos(point);

	rect = nameItem->boundingRect();
	rect.getRect(&rx, &ry, &nameWidth, &height);
	
	xdiff = queueWidth/2 - nameWidth/2;

	if (type == Queue)
		nameItem->moveBy(xdiff, 20);
	else if (type == Source)
		nameItem->moveBy(xdiff, 120);
	else if (type == Result)
		nameItem->moveBy(xdiff, 90);
}



/* ****************************************************************************
*
* DelilahSceneItem::inTypeSet - 
*/
void DelilahSceneItem::inTypeSet(const char* newType)
{
	if (inType)
		delete inType;

	inType = strdup(newType);
}



/* ****************************************************************************
*
* DelilahSceneItem::outTypeSet - 
*/
void DelilahSceneItem::outTypeSet(const char* newType)
{
	if (outType)
		delete outType;

	outType = strdup(newType);
}



/* ****************************************************************************
*
* DelilahSceneItem::disable - 
*/
void DelilahSceneItem::disable(void)
{
	float opacity;

	disabled = (disabled == true)? false : true;
	opacity  = (disabled == true)? 0.2 : 1;

	pixmapItem->setOpacity(opacity);
	nameItem->setOpacity(opacity);

	connectionMgr->setOpacity(this, opacity);

	if (type == Source)
	{
		QString txt;

		txt = scene->disableAction->text();

		if (txt == "Disable")
			scene->disableAction->setText("Enable");
		else
			scene->disableAction->setText("Disable");
		chainDisable();
	}

#if 0
	char* path;

	delete pixmap;
	scene->removeItem(pixmapItem);

	if (disabled)
	{
		if (type == Source)
			path = (char*) "images/sourceDisabled.png";
		else if (type == Result)
			path = (char*) "images/resultDisabled.png";
		else
			path = (char*) "images/queueDisabled.png";
	}
	else
	{
		if (type == Source)
			path = (char*) "images/Bomba.png";
		else if (type == Result)
			path = (char*) "images/Result.png";
		else
			path = (char*) "images/queue.png";
	}

	pixmap        = new QPixmap(path);
	pixmapItem    = scene->addPixmap(*pixmap);

	pixmapItem->stackBefore(nameItem);
	pixmapItem->setPos(xpos, ypos);
	nameCenter();
#endif
}



/* ****************************************************************************
*
* DelilahSceneItem::chainDisable - 
*/
void DelilahSceneItem::chainDisable(void)
{
	DelilahConnection* outV[20];
	int                outs;
	int                ix;

	LM_T(LmtSceneItemChain, ("Disabling '%s'", displayName));

	if (type != Source)
		disable();

	outs = connectionMgr->outgoingConnections(this, outV);
	LM_T(LmtSceneItemChain, ("'%s' has %d outgoing connections", displayName, outs));
	for (ix = 0; ix < outs; ix++)
	{
		LM_T(LmtSceneItemChain, ("Disabling connection to '%s'", outV[ix]->qToP->displayName));
		outV[ix]->disable();

		LM_T(LmtSceneItemChain, ("Recursive call to '%s'", outV[ix]->qToP->displayName));
		outV[ix]->qToP->chainDisable();
	}

	LM_T(LmtSceneItemChain, ("'%s' is done", displayName));
}



/* ****************************************************************************
*
* DelilahSceneItem::chainRemove - 
*/
void DelilahSceneItem::chainRemove(void)
{
	DelilahConnection* outV[20];
	int                outs;
	int                ix;

	LM_T(LmtSceneItemChain, ("Removing '%s'", displayName));
	outs = connectionMgr->outgoingConnections(this, outV);
	LM_T(LmtSceneItemChain, ("'%s' has %d outgoing connections", displayName, outs));
	for (ix = 0; ix < outs; ix++)
	{
		// LM_T(LmtSceneItemChain, ("Removing connection to '%s'", outV[ix]->qToP->displayName));
		// outV[ix]->remove();

		LM_T(LmtSceneItemChain, ("Recursive call to '%s'", outV[ix]->qToP->displayName));
		outV[ix]->qToP->chainRemove();
	}

	LM_T(LmtSceneItemChain, ("'%s' is done", displayName));

	if (type == Queue)
	{
		connectionMgr->remove(this);
		queueMgr->remove((DelilahQueue*) this);
	}
	else if (type == Result)
	{
		connectionMgr->remove(this);
		resultMgr->remove((DelilahResult*) this);
	}
	else
		sourceMgr->remove((DelilahSource*) this);
}



/* ****************************************************************************
*
* DelilahSceneItem::chainMove - 
*/
void DelilahSceneItem::chainMove(float dx, float dy)
{
	DelilahConnection* outV[20];
	int                outs;
	int                ix;

	LM_T(LmtSceneItemChain, ("Moving '%s'", displayName));
	outs = connectionMgr->outgoingConnections(this, outV);
	LM_T(LmtSceneItemChain, ("'%s' has %d outgoing connections", displayName, outs));
	for (ix = 0; ix < outs; ix++)
	{
		LM_T(LmtSceneItemChain, ("Recursive call to '%s'", outV[ix]->qToP->displayName));
		outV[ix]->qToP->chainMove(dx, dy);
	}

	LM_T(LmtSceneItemChain, ("'%s' is done", displayName));

	LM_T(LmtMove, ("Calling moveTo for '%s'", displayName));
	moveTo(dx, dy, true);
}
