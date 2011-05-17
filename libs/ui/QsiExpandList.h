#ifndef QSI_EXPAND_LIST_H
#define QSI_EXPAND_LIST_H

/* ****************************************************************************
*
* FILE                     QsiExpandList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include "QsiBox.h"             // QsiBox



namespace Qsi
{



/* ****************************************************************************
*
* - 
*/
class QsiManager;
class QsiBase;
class QsiBlock;



/* ****************************************************************************
*
* ExpandList - 
*/
class ExpandList : public QsiBox
{
private:
	char*      titleString;
	QsiBlock*  title;
	QsiBox*    memberBox;
	QsiBase**  member;
	int        members;
	int        autoExCo;
	bool       frame;
	int        xmargin;
	int        ymargin;
	int        memberSpace;

public:
	ExpandList(QsiManager* manager, QsiBox* owner, const char* _title, int x, int y, int _xmargin = 10, int _ymargin = 10, bool _frame = false);

	QsiBase*  addMember(const char* string);
	QsiBase*  addMember(QsiBase* _member);
	void      expand(void);
	void      compress(void);
	void      exCoOnButtonPress(bool _autoExCo);
	void      titleSet(const char* _title);
	void      menu(QsiFunction callback, char* mVec[]);
};

}

#endif
