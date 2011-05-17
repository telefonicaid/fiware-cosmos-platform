/* ****************************************************************************
*
* FILE                     QsiBlock.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 10 2011
*
*/
#include <QFont>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiAlignment.h"       // Alignment
#include "QsiBase.h"            // Base
#include "QsiBox.h"             // Box
#include "QsiManager.h"         // Manager
#include "QsiBlock.h"           // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* Block::Block - 
*/
Block::Block
(
	Manager*     manager,
	Box*         owner,
	Type         type,
	const char*  name,
	const char*  txt,
	int          x,
	int          y,
	int          width,
	int          height
) : Base(owner, type, name, x, y, width, height)
{
	char path[256];

	this->manager   = manager;
	
	w.vP      = NULL;
	proxy     = NULL;  // Should use gItem for the proxy ...
	gItemP    = NULL;

	movable   = true;
	boxMove   = true;
	expanded  = true;

	// As we haven't been moved yet ...
	// this->x = 0;
	// this->y = 0;

	for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
	{
		menuTitle[ix]  = NULL;
		menuFunc[ix]   = NULL;
		menuParam[ix]  = NULL;
	}

	menu = false;

	switch (type)
	{
	case Line:
		gItemP = manager->addLine(0, 0, width, height);
		break;

	case SimpleText:
		gItemP = manager->addSimpleText(txt);
		break;

	case Image:
		snprintf(path, sizeof(path), "%s/%s", manager->Home, txt);
		if (access(path, R_OK) != 0)
			LM_RVE(("pixmap path '%s' not readable", path));

		w.pixmap = new QPixmap(path);
		gItemP   = manager->addPixmap(*w.pixmap);
		if (width != -1)
			scale(width, height);
		break;

	case Label:
		w.label = new QLabel();
		proxy   = manager->addWidget(w.label);
		w.label->setText(txt);
		break;

	case Button:
		w.button = new QPushButton();
		proxy    = manager->addWidget(w.button);
		w.button->setText(txt);
		if (width != -1)
		   setSize(width, height);
		break;

	case Input:
		w.lineEdit = new QLineEdit();
		proxy      = manager->addWidget(w.lineEdit);
		break;

	case BoxItem:
		LM_X(1, ("Cannot create a Box ..."));

	case ExpandListItem:
		LM_X(1, ("Please use ExpandList"));
	}

	if (width == -1)
		LM_T(LmtCreate, ("Created %s '%s' at { %d, %d } (0x%x, 0x%x)", typeName(), name, x, y, proxy, gItemP));
	else
		LM_T(LmtCreate, ("Created %s '%s' at { %d, %d } %d x %d (0x%x, 0x%x)", typeName(), name, x, y, width, height, proxy, gItemP));

	//
	// Setting initial posistion to Absolute 0, 0
	//
	if (proxy != NULL)
		proxy->setPos(0, 0);
	else if (gItemP != NULL)
		gItemP->setPos(0, 0);
}



/* ****************************************************************************
*
* Block::~Block - 
*/
Block::~Block()
{
	LM_T(LmtRemove, ("Destroying %s '%s'", typeName(), name));

	if (gItemP)
	{
		LM_T(LmtRemove, ("Deleting gItemP"));
		delete gItemP;
	}

	if (proxy)
	{
		LM_T(LmtRemove, ("Deleting proxy"));
		delete proxy;
	}

	if (w.vP)
	{
		LM_T(LmtRemove, ("Deleting proxied widget"));
		delete w.lineEdit;
	}
}



/* ****************************************************************************
*
* scale - 
*/
void Block::scale(int width, int height)
{
	QPixmap  pmap;

	if (type != Image)
		LM_RVE(("Will only scale pixmaps"));
	
	if (w.vP != NULL)
		delete w.pixmap;

	w.pixmap = new QPixmap(w.pixmap->scaled(width, height, Qt::IgnoreAspectRatio));

	if (gItemP)
		delete gItemP;

	gItemP = manager->addPixmap(*w.pixmap);
	LM_TODO(("Move this pixmap back to its position"));
}



/* ****************************************************************************
*
* scale - 
*/
void Block::scale(int percentage)
{
	int      x;
	int      y;
	int      w;
	int      h;

	if (type != Image)
		LM_RVE(("Will only scale pixmaps"));

	geometry(&x, &y, &w, &h);
	scale(w * percentage / 100, h * percentage / 100);
}



/* ****************************************************************************
*
* - 
*/
void Block::setSize(int width, int height)
{
	if (type == Button)
		w.button->setFixedSize(width, height);
	else
		LM_W(("Block::setSize not implemented for Scene Items of type '%s'", typeName()));
}



/* ****************************************************************************
*
* moveRelative - relative positioning
*/
void Block::moveRelative(int x, int y)
{
	LM_T(LmtMove, ("Moving %s '%s' %d pixels in X-axis and %d pixels in Y-axis. Old pos: { %d, %d }, New pos: { %d, %d }",
				   typeName(), name, x, y, this->x, this->y, this->x + x, this->y + y));

	this->x += x;
	this->y += y;

	if (gItemP)
		gItemP->moveBy(x, y);
	if (proxy)
		proxy->moveBy(x, y);
}



/* ****************************************************************************
*
* moveAbsolute - absolute positioning
*/
void Block::moveAbsolute(int x, int y)
{
	LM_T(LmtAbsMove, ("Moving %s '%s' to absolute position { %d, %d }", typeName(), name, x, y));

	if (gItemP)
		gItemP->setPos(x, y);
	if (proxy)
		proxy->setPos(x, y);
}




/* ****************************************************************************
*
* align - 
*/
void Block::align(Alignment::Type type, Base* master, int margin)
{
	owner->align(master, type, this, margin);
}



/* ****************************************************************************
*
* hide - 
*/
void Block::hide(void)
{
	LM_T(LmtHide, ("Hiding %s '%s'", typeName(), name));
	gItemP->setVisible(false);
}



/* ****************************************************************************
*
* hideOthers - 
*/
void Block::hideOthers(void)
{
	LM_T(LmtHide, ("%s '%s' hiding others", typeName(), name));
	owner->hide();

	LM_T(LmtHide, ("%s '%s' showing itself", typeName(), name));
	show();
	owner->sizeChange(this);
	expanded = false;
}



/* ****************************************************************************
*
* showOthers - 
*/
void Block::showOthers(void)
{
	LM_T(LmtHide, ("%s '%s' showing others", typeName(), name));
	owner->show();
	expanded = true;
}



/* ****************************************************************************
*
* show - 
*/
void Block::show(void)
{
	LM_T(LmtHide, ("Showing %s '%s'", typeName(), name));
	gItemP->setVisible(true);
}



/* ****************************************************************************
*
* menuAdd - 
*/
void Block::menuAdd(const char* title, Function func, void* param)
{
	menu = true;

	for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
	{
		if (menuFunc[ix] != NULL)
			continue;

		menuTitle[ix]  = strdup(title);
		menuFunc[ix]   = func;
		menuParam[ix]  = param;
		return;
	}

	LM_W(("Maximum %d menu entries", QSI_MENU_ACTIONS));
}



/* ****************************************************************************
*
* menuClear - 
*/
void Block::menuClear(void)
{
	menu = false;

	for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
	{
		if (menuFunc[ix] == NULL)
			continue;
		
		free(menuTitle[ix]);
		menuTitle[ix]  = NULL;
		menuFunc[ix]   = NULL;
		menuParam[ix]  = NULL;
	}
}



/* ****************************************************************************
*
* geometry - 
*/
int Block::geometry(int* xP, int* yP, int* widthP, int* heightP)
{
	QRectF rect;

	if (isVisible() == false)
	{
		*xP       = x;  // was '0' and all almost worked
		*yP       = y;  // was '0' and all almost worked
		*widthP   = 0;
		*heightP  = 0;

		LM_T(LmtGeometry, ("  Invisible %s '%s' geometry: { %d, %d } %d x %d", typeName(), name, *xP, *yP, *widthP, *heightP));
		return -1;
	}

	switch (type)
	{
	case Image:
	case SimpleText:
	case Line:
		rect = gItemP->boundingRect();
		break;

	case Input:
	case Label:
	case Button:
		rect = proxy->boundingRect();
		break;

	case BoxItem:
		LM_X(1, ("Cannot be a Box!"));

	case ExpandListItem:
		LM_X(1, ("Cannot be an ExpandList:!"));
	}

	qreal rx;
	qreal ry;
	qreal rwidth;
	qreal rheight;

	rect.getRect(&rx, &ry, &rwidth, &rheight);
	*xP       = x;
	*yP       = y;
	*widthP   = rwidth;
	*heightP  = rheight;

	LM_T(LmtGeometry, ("  Visible %s '%s' geometry: { %d, %d } %d x %d", typeName(), name, *xP, *yP, *widthP, *heightP));

	return 0;
}



/* ****************************************************************************
*
* setMovable - 
*/
void Block::setMovable(bool movable)
{
	this->movable = movable;
	LM_T(LmtMove, ("%s '%s' set to %sMOVABLE", typeName(), name, (this->movable == true)? "" : "NOT "));
}



/* ****************************************************************************
*
* getMovable - 
*/
bool Block::getMovable(void)
{
	return movable;
}



/* ****************************************************************************
*
* setBoxMove - 
*/
void Block::setBoxMove(bool boxMove)
{
	this->boxMove = boxMove;
}



/* ****************************************************************************
*
* getBoxMove - 
*/
bool Block::getBoxMove(void)
{
	return boxMove;
}



/* ****************************************************************************
*
* isExpanded - 
*/
bool Block::isExpanded(void)
{
	return expanded;
}



/* ****************************************************************************
*
* isVisible - 
*/
bool Block::isVisible(void)
{
	if (gItemP != NULL)
		return gItemP->isVisible();
	else if (proxy != NULL)
		return proxy->isVisible();

	LM_E(("Block '%s': Both gItemP and proxy NULL - this can't be!", name));
	return false;
}



/* ****************************************************************************
*
* getText - 
*/
const char* Block::getText(void)
{
	if (type == Input)
		return w.lineEdit->text().toStdString().c_str();

	LM_W(("Scene item of type '%s' cannot give away text ...", typeName()));
	return NULL;
}



/* ****************************************************************************
*
* setText - 
*/
void Block::setText(const char* txt)
{
	if (type != SimpleText)
		LM_RVE(("Scene item of type '%s' cannot have its text set ...", typeName()));

	QGraphicsSimpleTextItem*  textItem = (QGraphicsSimpleTextItem*) gItemP;

	textItem->setText(txt);
}



/* ****************************************************************************
*
* setBold - 
*/
void Block::setBold(bool onOff)
{
	if (type != SimpleText)
		LM_RVE(("Scene item of type '%s' cannot change to Bold font ...", typeName()));

	QGraphicsSimpleTextItem*  textItem = (QGraphicsSimpleTextItem*) gItemP;
	QFont                     font     = textItem->font();

	font.setBold(onOff);
	textItem->setFont(font);
}



/* ****************************************************************************
*
* setItalic - 
*/
void Block::setItalic(bool onOff)
{
	if (type != SimpleText)
		LM_RVE(("Scene item of type '%s' cannot change to Italic font ...", typeName()));

	QGraphicsSimpleTextItem*  textItem = (QGraphicsSimpleTextItem*) gItemP;
	QFont                     font     = textItem->font();

	font.setItalic(onOff);
	textItem->setFont(font);
}



/* ****************************************************************************
*
* setColor - 
*/
void Block::setColor(int r, int g, int b, int a)
{
	if (type == SimpleText)
	{
		QGraphicsSimpleTextItem*  textItem = (QGraphicsSimpleTextItem*) gItemP;
		textItem->setBrush(QBrush(QColor(r, g, b, a)));
	}
	else if (type == Line)
	{
		QGraphicsLineItem* line = (QGraphicsLineItem*) gItemP;
		line->setPen(QColor(r, g, b, a));
	}
	else
        LM_RVE(("Scene item of type '%s' cannot change Color ...", typeName()));
}



/* ****************************************************************************
*
* setFont - 
*/
void Block::setFont(QFont font)
{
	if (type != SimpleText)
		LM_RVE(("Scene item of type '%s' cannot change Font ...", typeName()));

	QGraphicsSimpleTextItem*  textItem = (QGraphicsSimpleTextItem*) gItemP;
	textItem->setFont(font);
}

}
