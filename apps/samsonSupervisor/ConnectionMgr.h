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
class DelilahSceneItem;
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
	void                insert(DelilahScene* sceneP, DelilahSceneItem* from, DelilahSceneItem* to);
	DelilahConnection*  lookup(DelilahScene* sceneP, DelilahSceneItem* from, DelilahSceneItem* to);
	DelilahConnection*  lookup(QGraphicsItem* lineItem);
	int                 outgoingConnections(DelilahSceneItem* from);
	int                 incomingConnections(DelilahSceneItem* to);
	void                move(DelilahSceneItem* queue);

	void                remove(DelilahSceneItem* siP);
	void                remove(DelilahConnection* connection);
	void                removeAll(void);

private:
	DelilahConnection** conV;
	unsigned int        size;
};

#endif
