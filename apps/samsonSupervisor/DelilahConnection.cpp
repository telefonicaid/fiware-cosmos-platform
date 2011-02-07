/* ****************************************************************************
*
* FILE                     DelilahConnection.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include <QPen>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // sceneLayer1, ...
#include "misc.h"               // centerCoordinates
#include "DelilahSceneItem.h"   // DelilahSceneItem
#include "DelilahScene.h"       // DelilahScene
#include "DelilahConnection.h"  // Own interface



/* ****************************************************************************
*
* Constructor
*/
DelilahConnection::DelilahConnection(DelilahScene* sceneP, DelilahSceneItem* from, DelilahSceneItem* to)
{
	QPen    pen;

	disabled = false;
	scene    = sceneP;
	qFromP   = from;
	qToP     = to;
	lineItem = scene->addLine(0, 0, 0, 0);

	pen.setWidth(3);
	lineItem->setPen(pen);

#if 1
	lineItem->stackBefore(sceneLayer1);
#else
	lineItem->stackBefore(qFromP->pixmapItem);
	lineItem->stackBefore(qToP->pixmapItem);
#endif

	move();
}



/* ****************************************************************************
*
* Destructor
*/
DelilahConnection::~DelilahConnection()
{
	delete lineItem;
}



/* ****************************************************************************
*
* DelilahConnection::move
*/
void DelilahConnection::move(void)
{
	qreal   fromX;
	qreal   fromY;
	qreal   toX;
	qreal   toY;
	
	centerCoordinates(qFromP->pixmapItem, &fromX, &fromY);
	centerCoordinates(qToP->pixmapItem,   &toX,   &toY);

	lineItem->setLine(fromX, fromY, toX, toY);
}



/* ****************************************************************************
*
* DelilahConnection::setOpacity - 
*/
void DelilahConnection::setOpacity(float opacity)
{
	lineItem->setOpacity(opacity);
}



/* ****************************************************************************
*
* DelilahConnection::disable - 
*/
void DelilahConnection::disable(void)
{
	float opacity;

	disabled = (disabled == true)? false : true;
	opacity  = (disabled == true)? 0.2 : 1;

	setOpacity(opacity);
}
