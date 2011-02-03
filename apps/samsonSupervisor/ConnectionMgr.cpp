/* ****************************************************************************
*
* FILE                     ConnectionMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Popup.h"              // Popup
#include "DelilahScene.h"       // DelilahScene
#include "DelilahQueue.h"       // DelilahQueue
#include "DelilahConnection.h"  // DelilahConnection
#include "ConnectionMgr.h"      // Own interface



/* ****************************************************************************
*
* ConnectionMgr
*/
ConnectionMgr::ConnectionMgr(unsigned int size)
{
	this->size = size;

	conV = (DelilahConnection**) calloc(size, sizeof(DelilahConnection*));
	if (conV == NULL)
		LM_X(1, ("error allocating room for %d delilah connections", size));
}



/* ****************************************************************************
*
* ConnectionMgr::insert - 
*/
void ConnectionMgr::insert(DelilahScene* sceneP, DelilahQueue* from, DelilahQueue* to)
{
	unsigned int  ix;
	int           conIx = -1;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
		{
			conIx = ix;
			break;
		}
	}

	if (conIx == -1)
	{
		LM_TODO(("realloc connection vector ..."));
		new Popup("Out of connections", "Sorry, no room for more connections.\nTo fix this, you need to contact the developer (kzangeli@tid.es).");

		return;
	}

	conV[conIx] = new DelilahConnection(sceneP, from, to);
}



/* ****************************************************************************
*
* ConnectionMgr::remove - 
*/
void ConnectionMgr::remove(DelilahQueue* queue)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if ((conV[ix]->qFromP != queue) && (conV[ix]->qToP != queue))
			continue;

		delete conV[ix];
		conV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* ConnectionMgr::move - 
*/
void ConnectionMgr::move(DelilahQueue* queue)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if ((conV[ix]->qFromP != queue) && (conV[ix]->qToP != queue))
			continue;

		conV[ix]->move();
	}
}
