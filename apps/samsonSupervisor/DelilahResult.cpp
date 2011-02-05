/* ****************************************************************************
*
* FILE                     DelilahResult.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "DelilahScene.h"       // DelilahScene
#include "DelilahResult.h"      // Own interface



/* ****************************************************************************
*
* DelilahResult::DelilahResult - 
*/
DelilahResult::~DelilahResult()
{
}



/* ****************************************************************************
*
* DelilahResult::DelilahResult - 
*/
DelilahResult::DelilahResult(DelilahScene* sceneP, const char* imagePath, const char* displayNameP, int x, int y) :
DelilahSceneItem(DelilahSceneItem::Result, sceneP, imagePath, displayNameP, x, y)
{
}
