/* ****************************************************************************
*
* FILE                     QsiFrame.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 16 2011
*
*/
#include "QsiBox.h"             // QsiBox
#include "QsiManager.h"         // QsiManager
#include "QsiFrame.h"           // Own interface


namespace Qsi
{



/* ****************************************************************************
*
* Frame - 
*/
Frame::Frame(QsiBox* _box, const char* _title, int _padding)
{
	box        = _box;
	title      = strdup(_title);
	padding    = _padding;

	north      = NULL;
	west       = NULL;
	south      = NULL;
	east       = NULL;
	titleItem  = NULL;

	LM_M(("Calling paint method"));
	paint();
}



/* ****************************************************************************
*
* ~Frame - 
*/ 
Frame::~Frame()
{
	if (north)      delete north;
	if (south)      delete south;
	if (east)       delete east;
	if (west)       delete west;
	if (titleItem)  delete titleItem;
	if (title)      free(title);

	north = NULL;
	south = NULL;
	east  = NULL;
	west  = NULL;
	title = NULL;
}



/* ****************************************************************************
*
* paint - 
*/
void Frame::paint(void)
{
	int x, y, width, height;

	box->geometry(&x, &y, &width, &height);

	LM_M(("Got geometry of box '%s': { %d, %d } %dx%d", title, x, y, width, height));

	if (north == NULL)      north     = box->managerGet()->addLine(x - padding,         y - padding,          x + width + padding, y - padding);
	if (south == NULL)      south     = box->managerGet()->addLine(x - padding,         y + height + padding, x + width + padding, y + height + padding);
	if (east  == NULL)      east      = box->managerGet()->addLine(x + width + padding, y - padding,          x + width + padding, y + height + padding);
	if (west  == NULL)      west      = box->managerGet()->addLine(x - padding,         y - padding,          x - padding,         y + height + padding);
	if (titleItem == NULL)
	{
		titleItem = box->managerGet()->addSimpleText(title);
		titleItem->setPos(x - padding + 10, y - padding - 15);
	}
}

}
