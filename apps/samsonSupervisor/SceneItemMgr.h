#ifndef SCENEITEM_MGR_H
#define SCENEITEM_MGR_H

/* ****************************************************************************
*
* FILE                     SceneItemMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
class QGraphicsItem;
class DelilahSceneItem;



/* ****************************************************************************
*
* SceneItemMgr
*/
class SceneItemMgr
{
public:
	SceneItemMgr(unsigned int size);
	~SceneItemMgr();

	int                sceneItems();
	void               insert(DelilahSceneItem*  sceneitem);
	void               remove(DelilahSceneItem*  sceneitem);
	void               removeAll(void);
	DelilahSceneItem*  lookup(QGraphicsItem* itemP);
	void               move(int x, int y);

private:
	DelilahSceneItem** itemV;
	unsigned int       size;
};

#endif
