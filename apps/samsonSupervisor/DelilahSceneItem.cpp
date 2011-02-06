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
#include "DelilahSceneItem.h"       // Own interface



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
}



/* ****************************************************************************
*
* DelilahSceneItem::DelilahSceneItem - 
*/
DelilahSceneItem::DelilahSceneItem(Type type, DelilahScene* sceneP, const char* imagePath, const char* displayNameP, int x, int y)
{
	char itemName[32];

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

#if 0
	xpos          = (x == 0)? itemNo * 100 : x;
	ypos          = (y == 0)? itemNo *  50 : y;
#else
	xpos = x;
	ypos = y;
#endif

	pixmapItem    = scene->addPixmap(*pixmap);
	nameItem      = scene->addSimpleText(QString(displayName));
	
	moveTo(x, y);
	nameCenter();
}



/* ****************************************************************************
*
* moveTo - 
*/
void DelilahSceneItem::moveTo(int x, int y)
{
	pixmapItem->moveBy(x, y);
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
