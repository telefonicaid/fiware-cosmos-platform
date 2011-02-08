#ifndef RESULT_MGR_H
#define RESULT_MGR_H

/* ****************************************************************************
*
* FILE                     ResultMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
class QGraphicsItem;
class DelilahResult;



/* ****************************************************************************
*
* ResultMgr
*/
class ResultMgr
{
public:
	ResultMgr(unsigned int size);
	~ResultMgr();

	int             results();
	void            insert(DelilahResult* result);
	void            remove(DelilahResult* result);
	void            removeAll(void);
	DelilahResult*  lookup(QGraphicsItem* itemP);
	// DelilahResult*  lookup(int cardinal);
	void            move(int x, int y);

private:
	DelilahResult** resultV;
	unsigned int    size;
};

#endif
