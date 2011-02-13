#ifndef SOURCE_MGR_H
#define SOURCE_MGR_H

/* ****************************************************************************
*
* FILE                     SourceMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
class QGraphicsItem;
class DelilahSource;



/* ****************************************************************************
*
* SourceMgr
*/
class SourceMgr
{
public:
	SourceMgr(unsigned int size);
	~SourceMgr();

	int             sources();
	void            insert(DelilahSource* source);
	void            remove(DelilahSource* source);
	void            removeAll(void);
	DelilahSource*  lookup(QGraphicsItem* itemP);
	// DelilahSource*  lookup(int cardinal);
	void            move(int x, int y);
	void            markMoved(bool moved);

private:
	DelilahSource** sourceV;
	unsigned int    size;
};

#endif
