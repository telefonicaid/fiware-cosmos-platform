#ifndef CONNECTION_MGR_H
#define CONNECTION_MGR_H

/* ****************************************************************************
*
* FILE                     ConnectionMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
class DelilahScene;
class DelilahQueue;
class DelilahConnection;



/* ****************************************************************************
*
* ConnectionMgr
*/
class ConnectionMgr
{
public:
	ConnectionMgr(unsigned int size);

	int                 connections(void);
	void                insert(DelilahScene* sceneP, DelilahQueue* from, DelilahQueue* to);
	DelilahConnection*  lookup(DelilahScene* sceneP, DelilahQueue* from, DelilahQueue* to);
	DelilahConnection*  lookup(QGraphicsItem* lineItem);
	void                move(DelilahQueue* queue);

	void                remove(DelilahQueue* queue);
	void                remove(DelilahConnection* connection);
	void                removeAll(void);

private:
	DelilahConnection** conV;
	unsigned int        size;
};

#endif
