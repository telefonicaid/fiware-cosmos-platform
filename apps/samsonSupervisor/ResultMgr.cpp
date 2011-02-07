/* ****************************************************************************
*
* FILE                     ResultMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Popup.h"              // Popup
#include "DelilahResult.h"       // DelilahResult
#include "ResultMgr.h"           // Own interface



/* ****************************************************************************
*
* ResultMgr
*/
ResultMgr::ResultMgr(unsigned int size)
{
	this->size = size;

	resultV = (DelilahResult**) calloc(size, sizeof(DelilahResult*));
	if (resultV == NULL)
		LM_X(1, ("error allocating room for %d delilah results", size));
}



/* ****************************************************************************
*
* ResultMgr::results - 
*/
int ResultMgr::results(void)
{
	unsigned int  ix;
	int           resultNo = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (resultV[ix] != NULL)
			++resultNo;
	}

	return resultNo;
}



/* ****************************************************************************
*
* ResultMgr::insert - 
*/
void ResultMgr::insert(DelilahResult* result)
{
	unsigned int  ix;
	int           resultIx = -1;

	for (ix = 0; ix < size; ix++)
	{
		if (resultV[ix] == NULL)
		{
			resultIx = ix;
			break;
		}
	}

	if (resultIx == -1)
	{
		LM_TODO(("realloc result vector ..."));
		new Popup("Out of results", "Sorry, no room for more results.\nTo fix this, you need to contact the developer (kzangeli@tid.es).");

		return;
	}

	LM_T(LmtResultMgr, ("Inserting result '%s'", result->displayName));
	resultV[resultIx] = result;
}



/* ****************************************************************************
*
* ResultMgr::remove - 
*/
void ResultMgr::remove(DelilahResult* result)
{
	unsigned int ix;

	LM_T(LmtResultMgr, ("Trying to remove result '%s'", result->displayName));
	for (ix = 0; ix < size; ix++)
	{
		if (resultV[ix] != result)
			continue;

		LM_T(LmtResultMgr, ("Removing result '%s'", result->displayName));
		delete resultV[ix];
		resultV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* lookup - 
*/
DelilahResult* ResultMgr::lookup(QGraphicsItem* itemP)
{
	unsigned int ix;

	if (itemP == NULL)
		return NULL;

	for (ix = 0; ix < size; ix++)
	{
		if (resultV[ix] == NULL)
			continue;

		if ((QGraphicsItem*) resultV[ix]->pixmapItem == itemP)
		   return resultV[ix];
		if ((QGraphicsItem*) resultV[ix]->nameItem == itemP)
		   return resultV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* lookup - 
*/
DelilahResult* ResultMgr::lookup(int cardinal)
{
	unsigned int ix;
	int          qIx = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (resultV[ix] == NULL)
			continue;

		++qIx;

		if (cardinal == qIx)
			return resultV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* removeAll - 
*/
void ResultMgr::removeAll(void)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (resultV[ix] == NULL)
			continue;

		delete resultV[ix];
		resultV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* move - 
*/
void ResultMgr::move(int x, int y)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (resultV[ix] == NULL)
			continue;

		LM_M(("Calling moveTo for '%s'", resultV[ix]->displayName));
		resultV[ix]->moveTo(x, y);
	}
}
