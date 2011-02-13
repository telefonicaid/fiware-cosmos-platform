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
	inTypeSet("Undefined", 0);

	outTypeIndex  = 0;
	outTypeSet("Undefined", 0);

	commandIndex  = 0;
	command = strdup("No command");
}



/* ****************************************************************************
*
* DelilahQueue::commandSet - 
*/
void DelilahQueue::commandSet(const char* newCommand, int index)
{
	if (command)
		delete command;

	command      = strdup(newCommand);
	commandIndex = index;
}
