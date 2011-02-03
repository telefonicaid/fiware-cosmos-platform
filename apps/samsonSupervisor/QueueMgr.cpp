/* ****************************************************************************
*
* FILE                     QueueMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Popup.h"              // Popup
#include "DelilahQueue.h"       // DelilahQueue
#include "QueueMgr.h"           // Own interface



/* ****************************************************************************
*
* QueueMgr
*/
QueueMgr::QueueMgr(unsigned int size)
{
	this->size = size;

	queueV = (DelilahQueue**) calloc(size, sizeof(DelilahQueue*));
	if (queueV == NULL)
		LM_X(1, ("error allocating room for %d delilah queues", size));
}



/* ****************************************************************************
*
* QueueMgr::insert - 
*/
void QueueMgr::insert(DelilahQueue* queue)
{
	unsigned int  ix;
	int           queueIx = -1;

	for (ix = 0; ix < size; ix++)
	{
		if (queueV[ix] == NULL)
		{
			queueIx = ix;
			break;
		}
	}

	if (queueIx == -1)
	{
		LM_TODO(("realloc queue vector ..."));
		new Popup("Out of queues", "Sorry, no room for more queues.\nTo fix this, you need to contact the developer (kzangeli@tid.es).");

		return;
	}

	queueV[queueIx] = queue;
}



/* ****************************************************************************
*
* QueueMgr::remove - 
*/
void QueueMgr::remove(DelilahQueue* queue)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (queueV[ix] != queue)
			continue;

		delete queueV[ix];
		queueV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* lookup - 
*/
DelilahQueue* QueueMgr::lookup(QGraphicsItem* itemP)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (queueV[ix] == NULL)
			continue;

		if ((QGraphicsItem*) queueV[ix]->pixmapItem == itemP)
		   return queueV[ix];
		if ((QGraphicsItem*) queueV[ix]->nameItem == itemP)
		   return queueV[ix];
	}

	return NULL;
}
