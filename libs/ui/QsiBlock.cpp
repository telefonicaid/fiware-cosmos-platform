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

#include "QsiAlignment.h"       // QsiAlignment
#include "QsiBase.h"            // QsiBase
#include "QsiBox.h"             // QsiBox
#include "QsiManager.h"         // QsiManager
#include "QsiBlock.h"           // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* QsiBlock::QsiBlock - 
*/
QsiBlock::QsiBlock
(
	QsiManager*  manager,
	QsiBox*      owner,
	QsiType      type,
	const char*  name,
	const char*  txt,
	int          x,
	int          y,
	int          width,
	int          height
) : QsiBase(owner, type, name, x, y, width, height)
{
	char path[256];

	this->manager   = manager;
	
	w.vP      = NULL;
	proxy     = NULL;  // Should use gItem for the proxy ...
	gItemP    = NULL;

	movable   = false;
	boxMove   = false;
	expanded  = true;

	// As we haven't been moved yet ...
	// this->x = 0;
	// this->y = 0;

	for (int ix = 0; ix < 10; ix++)
	{
		menuTitle[ix]  = NULL;
		menuFunc[ix]   = NULL;
		menuParam[ix]  = NULL;
	}

	menu = false;

	switch (type)
	{
	case Line:
		gItemP = manager->addLine(x, y, width, height);
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

	case Box:
		LM_X(1, ("Cannot create a Box ..."));
	}

	if (width == -1)
		LM_T(LmtCreate, ("Created %s '%s' at { %d, %d } (0x%x, 0x%x)", typeName(), name, x, y, proxy, gItemP));
	else
		LM_T(LmtCreate, ("Created %s '%s' at { %d, %d } %d x %d (0x%x, 0x%x)", typeName(), name, x, y, width, height, proxy, gItemP));
}



/* ****************************************************************************
*
* QsiBlock::~QsiBlock - 
*/
QsiBlock::~QsiBlock()
{
	if (gItemP)
		delete gItemP;
	if (proxy)
		delete proxy;
	if (w.vP)
		delete w.lineEdit;
}



/* ****************************************************************************
*
* scale - 
*/
void QsiBlock::scale(int width, int height)
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
void QsiBlock::scale(int percentage)
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
void QsiBlock::setSize(int width, int height)
{
	if (type == Button)
		w.button->setFixedSize(width, height);
	else
		LM_W(("QsiBlock::setSize not implemented for Scene Items of type '%s'", typeName()));
}



/* ****************************************************************************
*
* QsiBlock::moveRelative - relative positioning
*/
void QsiBlock::moveRelative(int x, int y)
{
	this->x += x;
	this->y += y;

	LM_T(LmtMove, ("Moving %s '%s' to relative position { %d, %d }", typeName(), name, this->x, this->y));

	if (gItemP)
		gItemP->moveBy(x, y);
	if (proxy)
		proxy->moveBy(x, y);
}



/* ****************************************************************************
*
* moveAbsolute - absolute positioning
*/
void QsiBlock::moveAbsolute(int x, int y)
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
void QsiBlock::align(Alignment::Type type, QsiBase* master, int margin)
{
	owner->align(master, type, this, margin);
}



/* ****************************************************************************
*
* QsiBlock::hide - 
*/
void QsiBlock::hide(void)
{
	gItemP->setVisible(false);
}



/* ****************************************************************************
*
* QsiBlock::hideOthers - 
*/
void QsiBlock::hideOthers(void)
{
	owner->hide();
	gItemP->setVisible(true);
	expanded = false;
}



/* ****************************************************************************
*
* QsiBlock::showOthers - 
*/
void QsiBlock::showOthers(void)
{
	owner->show();
	expanded = true;
}



/* ****************************************************************************
*
* QsiBlock::show - 
*/
void QsiBlock::show(void)
{
	gItemP->setVisible(true);
}



/* ****************************************************************************
*
* QsiBlock::menuAdd - 
*/
void QsiBlock::menuAdd(const char* title, QsiFunction func, void* param)
{
	menu = true;

	for (int ix = 0; ix < 10; ix++)
	{
		if (menuFunc[ix] != NULL)
			continue;

		menuTitle[ix]  = strdup(title);
		menuFunc[ix]   = func;
		menuParam[ix]  = param;
		break;
	}

	LM_W(("Maximum 10 menu entries"));
}



/* ****************************************************************************
*
* QsiBlock::menuClear - 
*/
void QsiBlock::menuClear()
{
	menu = false;

	for (int ix = 0; ix < 10; ix++)
	{
		if (menuFunc[ix] == NULL)
			continue;
		
		free(menuTitle[ix]);
		menuTitle[ix]  = NULL;
		menuFunc[ix]   = NULL;
		menuParam[ix]  = NULL;
	}

	LM_W(("Maximum 10 menu entries"));
}



/* ****************************************************************************
*
* geometry - 
*/
void QsiBlock::geometry(int* xP, int* yP, int* widthP, int* heightP)
{
	QRectF rect;

	if (isVisible() == false)
	{
		*xP       = 0;
		*yP       = 0;
		*widthP   = 0;
		*heightP  = 0;

		LM_T(LmtGeometry, ("  Invisible %s '%s' geometry: { %d, %d } %d x %d", typeName(), name, *xP, *yP, *widthP, *heightP));
		return;
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

	case Box:
		LM_X(1, ("Cannot be a Box!"));
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

	LM_T(LmtGeometry, ("  Visible %s '%s' geometry: { %d, %d } %d x %d (getRect returned { %d, %d })", typeName(), name, *xP, *yP, *widthP, *heightP, rx, ry));
}



/* ****************************************************************************
*
* setMovable - 
*/
void QsiBlock::setMovable(bool movable)
{
	this->movable = movable;
}



/* ****************************************************************************
*
* getMovable - 
*/
bool QsiBlock::getMovable(void)
{
	return movable;
}



/* ****************************************************************************
*
* setBoxMove - 
*/
void QsiBlock::setBoxMove(bool boxMove)
{
	this->boxMove = boxMove;
}



/* ****************************************************************************
*
* getBoxMove - 
*/
bool QsiBlock::getBoxMove(void)
{
	return boxMove;
}



/* ****************************************************************************
*
* isExpanded - 
*/
bool QsiBlock::isExpanded(void)
{
	return expanded;
}



/* ****************************************************************************
*
* isVisible - 
*/
bool QsiBlock::isVisible(void)
{
	if (gItemP != NULL)
		return gItemP->isVisible();
	else if (proxy != NULL)
		return proxy->isVisible();

	LM_E(("QsiBlock '%s': Both gItemP and proxy NULL - this can't be!", name));
	return false;
}



/* ****************************************************************************
*
* getText - 
*/
const char* QsiBlock::getText(void)
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
void QsiBlock::setText(const char* txt)
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
void QsiBlock::setBold(bool onOff)
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
void QsiBlock::setItalic(bool onOff)
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
void QsiBlock::setColor(int r, int g, int b, int a)
{
	if (type != SimpleText)
        LM_RVE(("Scene item of type '%s' cannot change Color ...", typeName()));

	QGraphicsSimpleTextItem*  textItem = (QGraphicsSimpleTextItem*) gItemP;

	textItem->setBrush(QBrush(QColor(r, g, b, a)));
}



/* ****************************************************************************
*
* setFont - 
*/
void QsiBlock::setFont(QFont font)
{
	if (type != SimpleText)
		LM_RVE(("Scene item of type '%s' cannot change Font ...", typeName()));

	QGraphicsSimpleTextItem*  textItem = (QGraphicsSimpleTextItem*) gItemP;
	textItem->setFont(font);
}

}
