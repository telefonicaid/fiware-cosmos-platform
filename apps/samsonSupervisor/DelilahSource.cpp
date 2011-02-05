/* ****************************************************************************
*
* FILE                     DelilahSource.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <stdio.h>             // snprintf

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "DelilahScene.h"       // DelilahScene
#include "DelilahSource.h"      // Own interface



/* ****************************************************************************
*
* DelilahSource::DelilahSource - 
*/
DelilahSource::~DelilahSource()
{
}



/* ****************************************************************************
*
* DelilahSource::DelilahSource - 
*/
DelilahSource::DelilahSource(DelilahScene* sceneP, const char* imagePath, const char* displayNameP, int x, int y) :
DelilahSceneItem(DelilahSceneItem::Source, sceneP, imagePath, displayNameP, x, y)
{
	outTypeIndex  = 0;
	outType       = strdup("Undefined");
}



/* ****************************************************************************
*
* DelilahSource::outTypeSet - 
*/
void DelilahSource::outTypeSet(const char* newType)
{
	if (outType)
		delete outType;

	outType = strdup(newType);
}
