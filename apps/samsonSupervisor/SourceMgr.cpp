/* ****************************************************************************
*
* FILE                     SourceMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Popup.h"              // Popup
#include "DelilahSource.h"       // DelilahSource
#include "SourceMgr.h"           // Own interface



/* ****************************************************************************
*
* SourceMgr
*/
SourceMgr::SourceMgr(unsigned int size)
{
	this->size = size;

	sourceV = (DelilahSource**) calloc(size, sizeof(DelilahSource*));
	if (sourceV == NULL)
		LM_X(1, ("error allocating room for %d delilah sources", size));
}



/* ****************************************************************************
*
* SourceMgr::sources - 
*/
int SourceMgr::sources(void)
{
	unsigned int  ix;
	int           sourceNo = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (sourceV[ix] != NULL)
			++sourceNo;
	}

	return sourceNo;
}



/* ****************************************************************************
*
* SourceMgr::insert - 
*/
void SourceMgr::insert(DelilahSource* source)
{
	unsigned int  ix;
	int           sourceIx = -1;

	for (ix = 0; ix < size; ix++)
	{
		if (sourceV[ix] == NULL)
		{
			sourceIx = ix;
			break;
		}
	}

	if (sourceIx == -1)
	{
		LM_TODO(("realloc source vector ..."));
		new Popup("Out of sources", "Sorry, no room for more sources.\nTo fix this, you need to contact the developer (kzangeli@tid.es).");

		return;
	}

	LM_T(LmtSourceMgr, ("Inserting source '%s'", source->displayName));
	sourceV[sourceIx] = source;
}



/* ****************************************************************************
*
* SourceMgr::remove - 
*/
void SourceMgr::remove(DelilahSource* source)
{
	unsigned int ix;

	LM_T(LmtSourceMgr, ("Trying to remove source '%s'", source->displayName));
	for (ix = 0; ix < size; ix++)
	{
		if (sourceV[ix] != source)
			continue;

		LM_T(LmtSourceMgr, ("Removing source '%s'", source->displayName));
		delete sourceV[ix];
		sourceV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* lookup - 
*/
DelilahSource* SourceMgr::lookup(QGraphicsItem* itemP)
{
	unsigned int ix;

	if (itemP == NULL)
		return NULL;

	for (ix = 0; ix < size; ix++)
	{
		if (sourceV[ix] == NULL)
			continue;

		if ((QGraphicsItem*) sourceV[ix]->pixmapItem == itemP)
		   return sourceV[ix];
		if ((QGraphicsItem*) sourceV[ix]->nameItem == itemP)
		   return sourceV[ix];
	}

	return NULL;
}


#if 0
/* ****************************************************************************
*
* lookup - 
*/
DelilahSource* SourceMgr::lookup(int cardinal)
{
	unsigned int ix;
	int          qIx = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (sourceV[ix] == NULL)
			continue;

		++qIx;

		if (cardinal == qIx)
			return sourceV[ix];
	}

	return NULL;
}
#endif


/* ****************************************************************************
*
* removeAll - 
*/
void SourceMgr::removeAll(void)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (sourceV[ix] == NULL)
			continue;

		delete sourceV[ix];
		sourceV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* move - 
*/
void SourceMgr::move(int x, int y)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (sourceV[ix] == NULL)
			continue;

		LM_T(LmtMove, ("Calling moveTo for '%s'", sourceV[ix]->displayName));
		sourceV[ix]->moveTo(x, y);
	}
}
