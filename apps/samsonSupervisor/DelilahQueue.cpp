/* ****************************************************************************
*
* FILE                     DelilahQueue.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <stdio.h>             // snprintf

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // connectionMgr, ...
#include "misc.h"               // centerCoordinates
#include "DelilahScene.h"       // DelilahScene
#include "DelilahQueue.h"       // Own interface



/* ****************************************************************************
*
* DelilahQueue::DelilahQueue - 
*/
DelilahQueue::~DelilahQueue()
{
}



/* ****************************************************************************
*
* DelilahQueue::DelilahQueue - 
*/
DelilahQueue::DelilahQueue(DelilahScene* sceneP, const char* imagePath, const char* displayNameP, int x, int y) :
DelilahSceneItem(DelilahSceneItem::Queue, sceneP, imagePath, displayNameP, x, y)
{
	inTypeIndex   = 0;
	outTypeIndex  = 0;
	inType        = strdup("Undefined");
	outType       = strdup("Undefined");
}



/* ****************************************************************************
*
* DelilahQueue::inTypeSet - 
*/
void DelilahQueue::inTypeSet(const char* newType)
{
	if (inType)
		delete inType;

	inType = strdup(newType);
}



/* ****************************************************************************
*
* DelilahQueue::outTypeSet - 
*/
void DelilahQueue::outTypeSet(const char* newType)
{
	if (outType)
		delete outType;

	outType = strdup(newType);
}
