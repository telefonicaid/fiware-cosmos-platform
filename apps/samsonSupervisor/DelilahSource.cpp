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
	inTypeSet("Not used", 0);
	sourceFileName = NULL;

	outTypeK = NULL;
	outTypeV = NULL;
}



/* ****************************************************************************
*
* DelilahSource::sourceFileNameSet - 
*/
void DelilahSource::sourceFileNameSet(const char* newName)
{
	if (sourceFileName)
		delete sourceFileName;

	sourceFileName = strdup(newName);
}



/* ****************************************************************************
*
* DelilahSource::outTypeSet - 
*/
void DelilahSource::outTypeSet(const char* newTypeK, int indexK, const char* newTypeV, int indexV)
{
	if (outTypeK)
		delete outTypeK;

	outTypeKIndex = indexK;
	outTypeK      = strdup(newTypeK);


	if (outTypeV)
		delete outTypeV;

	outTypeVIndex = indexV;
	outTypeV      = strdup(newTypeV);
}
