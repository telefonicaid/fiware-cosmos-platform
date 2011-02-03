#ifndef QUEUE_MGR_H
#define QUEUE_MGR_H

/* ****************************************************************************
*
* FILE                     QueueMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
class QGraphicsItem;
class DelilahQueue;



/* ****************************************************************************
*
* QueueMgr
*/
class QueueMgr
{
public:
	QueueMgr(unsigned int size);
	~QueueMgr();

	void           insert(DelilahQueue*  queue);
	void           remove(DelilahQueue*  queue);
	DelilahQueue*  lookup(QGraphicsItem* itemP);

private:
	DelilahQueue** queueV;
	unsigned int   size;
};

#endif
