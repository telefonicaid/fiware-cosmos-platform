/* ****************************************************************************
*
* FILE                     QsiExpandList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include <QGraphicsSimpleTextItem>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiManager.h"         // QsiManager
#include "QsiBase.h"            // QsiBase
#include "QsiBox.h"             // QsiBox
#include "QsiBlock.h"           // QsiBlock
#include "QsiExpandList.h"      // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* ExpandList - 
*/
ExpandList::ExpandList(QsiManager* manager, QsiBox* owner, const char* _title, int x, int y, int _xmargin, int _ymargin, bool _frame) : QsiBox(manager, owner, _title, x, y)
{
	this->typeSet(ExpandListItem);

	xmargin     = _xmargin;
	ymargin     = _ymargin;
	title       = (QsiBlock*) textAdd("title",    _title, x, y);
	memberBox   = (QsiBox*)   boxAdd("memberBox", x + xmargin, y + ymargin);
	memberSpace = 15;

	owner->add(this);
}



/* ****************************************************************************
*
* addMember - 
*/
QsiBase* ExpandList::addMember(const char* string)
{
	QsiBase* sMember;
	int      gx, gy, gwidth, gheight;

	memberBox->geometry(&gx, &gy, &gwidth, &gheight);

	sMember = memberBox->textAdd(string, string, 0, gheight + memberSpace);

	return sMember;
}



/* ****************************************************************************
*
* addMember - 
*/
QsiBase* ExpandList::addMember(QsiBase* _member)
{
	int gx, gy, gwidth, gheight;

	memberBox->geometry(&gx, &gy, &gwidth, &gheight);
	_member->xSet(0);
	_member->ySet(gheight + memberSpace);
	add(_member);

	return _member;
}



/* ****************************************************************************
*
* expand - 
*/
void ExpandList::expand(void)
{
	memberBox->show();
}



/* ****************************************************************************
*
* compress - 
*/
void ExpandList::compress(void)
{
	memberBox->hide();
}



/* ****************************************************************************
*
* exCoOnButtonPress - 
*/
void ExpandList::exCoOnButtonPress(bool _autoExCo)
{
	autoExCo = _autoExCo;
}



/* ****************************************************************************
*
* titleSet - 
*/
void ExpandList::titleSet(const char* _title)
{
	QGraphicsSimpleTextItem* qtitle = (QGraphicsSimpleTextItem*) title->gItemP;

	qtitle->setText(_title);
}



/* ****************************************************************************
*
* menu - 
*/
void ExpandList::menu(QsiFunction callback, char* mVec[])
{
	for (int ix = 0; mVec[ix] != NULL; ix++)
		LM_M(("mVec[%d]: '%s'", ix, mVec[ix]));

	LM_W(("Sorry, not implemented"));
}

}
