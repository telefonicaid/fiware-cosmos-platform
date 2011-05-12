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

#include "QsiManager.h"         // QsiManager
#include "QsiBase.h"            // QsiBase
#include "QsiBlock.h"           // Own interface


namespace Qsi
{



/* ****************************************************************************
*
* QsiBlock::QsiBlock - 
*/
QsiBlock::QsiBlock(QsiManager* _manager, QsiBlockType _type, const char* name, const char* txt, int _x, int _y, int width, int height) : QsiBase(name, _x, _y)
{
	char path[256];

	manager   = _manager;
	type      = _type;
	
	w.vP      = NULL;
	proxy     = NULL;  // Should use gItem for the proxy ...
	gItemP    = NULL;

	movable   = false;
	groupPrev = NULL;
	groupNext = NULL;

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
		gItemP = manager->addLine(_x, _y, width, height);
		manager->add(this);
		setPos(_x, _y);
		// move();
		return;
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
	}

	manager->add(this);
	setPos(manager->x, manager->y);
	move();
}



/* ****************************************************************************
*
* QsiBlock::~QsiBlock - 
*/
QsiBlock::~QsiBlock()
{
	delete gItemP;
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
	setPos(manager->x, manager->y);
    move();
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

	geometryGet(&x, &y, &w, &h, true);
	scale(w * percentage / 100, h * percentage / 100);
}



/* ****************************************************************************
*
* QsiBlock::setPos - 
*/
void QsiBlock::setPos(int x, int y)
{
	xpos = x;
	ypos = y;

	LM_M(("Set position { %d, %d } for '%s'", xpos, ypos, name));

	if (gItemP)
		gItemP->setPos(xpos, ypos);
	if (proxy)
		proxy->setPos(xpos, ypos);
}



/* ****************************************************************************
*
* - 
*/
void QsiBlock::setSize(int width, int height)
{
	if (type == Button)
	{
		w.button->setFixedSize(width, height);
	}
	else
	{
		LM_W(("QsiBlock::setSize not implemented for Scene Items of type '%s'", typeName()));
	}
}



/* ****************************************************************************
*
* QsiBlock::move - 
*/
void QsiBlock::move(void)
{
	xpos += x;
	ypos += y;

	if (gItemP)
		gItemP->moveBy(x, y);
	if (proxy)
		proxy->moveBy(x, y);
}



/* ****************************************************************************
*
* move - 
*/
void QsiBlock::move(int _x, int _y)
{
	x = _x;
	y =_y;

	move();
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
* QsiBlock::show - 
*/
void QsiBlock::show(void)
{
	gItemP->setVisible(true);
}



/* ****************************************************************************
*
* expand - 
*/
void QsiBlock::expand(void)
{
	QsiBlock* leader = groupLeader();
	QsiBlock* si;

	if (leader != this)
		LM_RVE(("Sorry, only group leaders can be EXPANDED"));

	si = groupNext;
	while (si != NULL)
	{
		si->show();
		si = si->groupNext;
	}
}



/* ****************************************************************************
*
* compress - 
*/
void QsiBlock::compress(void)
{
	QsiBlock* leader = groupLeader();
	QsiBlock* si;

	if (leader != this)
		LM_RVE(("Sorry, only group leaders can be COMPRESSED"));

	si = groupNext;
	while (si != NULL)
	{
		si->hide();
		si = si->groupNext;
	}
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
* typeName - 
*/
const char* QsiBlock::typeName(void)
{
	switch (type)
	{
	case SimpleText:       return "SimpleText";
	case Line:             return "Line";
	case Image:            return "Image";
	case Label:            return "Label";
	case Button:           return "Button";
	case Input:            return "Input";
	}

	return "Unknown Type";
}



/* ****************************************************************************
*
* setFont - 
*/
void QsiBlock::setFont(QFont* fontP)
{
	font = *fontP;
	LM_W(("Re-add text to manager"));
	LM_W(("Sorry, not implemented"));
}



/* ****************************************************************************
*
* setFontSize - 
*/
void QsiBlock::setFontSize(int size)
{
	size = 10;
	LM_W(("Sorry, not implemented"));
}



/* ****************************************************************************
*
* setFontColor - 
*/
void QsiBlock::setFontColor(int color)
{
	color = 0;
	LM_W(("Sorry, not implemented"));
}



/* ****************************************************************************
*
* groupGeometryGet - 
*
* QsiBlock::groupGeometryGet
*/
void QsiBlock::groupGeometryGet(int* xP, int* yP, int* widthP, int* heightP)
{
	QsiBlock*  si     = groupLeader();
	int         xMin   = 0x7FFFFFFF;
	int         yMin   = 0x7FFFFFFF;
	int         xMax   = 0;
	int         yMax   = 0;

	while (si != NULL)
	{
		int x;
		int y;
		int width;
		int height;

		if (si->isVisible())
		{
			si->geometryGet(&x, &y, &width, &height, true);
			if (x < xMin)
				xMin = x;
			if (y < yMin)
				yMin = y;
			if ((x + width) > xMax)
				xMax = x + width;
			if ((y + height) > yMax)
				yMax = y + height;

			LM_M(("Current group gemetry: { %d, %d } xMax: %d, yMax: %d", xMin, yMin, xMax, yMax));
		}
		si = si->groupNext;
	}

	*xP      = xMin;
	*yP      = yMin;
	*widthP  = (xMax - xMin);
	*heightP = (yMax - yMin);
}



/* ****************************************************************************
*
* geometryGet - 
*/
void QsiBlock::geometryGet(int* xP, int* yP, int* widthP, int* heightP, bool force)
{
	QRectF rect;

	if ((inGroup() == TRUE) && (force == FALSE))
		return groupGeometryGet(xP, yP, widthP, heightP);

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
	}

	qreal rx;
	qreal ry;
	qreal rwidth;
	qreal rheight;

	rect.getRect(&rx, &ry, &rwidth, &rheight);
	*xP       = xpos;
	*yP       = ypos;
	*widthP   = rwidth;
	*heightP  = rheight;

	LM_M(("Geometry for '%s': { %d, %d } %dx%d", name, xpos, ypos, *widthP, *heightP));
}



/* ****************************************************************************
*
* groupLeader - 
*/
QsiBlock* QsiBlock::groupLeader(void)
{
	QsiBlock* siP = this;

	while (siP->groupPrev != NULL)
		siP = siP->groupPrev;

	return siP;	  
}



/* ****************************************************************************
*
* inGroup - 
*/
bool QsiBlock::inGroup(void)
{
	QsiBlock* leader = groupLeader();

	if ((leader == this) && (groupNext == NULL))
		return FALSE;

	return TRUE;
}



/* ****************************************************************************
*
* setMovable - 
*/
void QsiBlock::setMovable(bool _movable)
{
	movable = _movable;
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
* align - align scene item to another (s2) 
*/
void QsiBlock::align(Alignment how, QsiBlock* s2, int padding)
{
	int x1;
	int y1;
	int w1;
	int h1;
	int x2;
	int y2;
	int w2;
	int h2;
	int xNew;
	int yNew;

	geometryGet(&x1, &y1, &w1, &h1);
	s2->geometryGet(&x2, &y2, &w2, &h2);

	LM_M(("Geometry for    '%s': { %d, %d } %dx%d", name,     x1, y1, w1, h1));
	LM_M(("Geometry for s2 '%s': { %d, %d } %dx%d", s2->name, x2, y2, w2, h2));

	if (how == Under)
	{
		xNew = x2;
		yNew = y2 + h2 + padding;
	}
	else if (how == Over)
	{
		xNew = x2;
		yNew = y2 - (h1 + padding);
	}
	else if (how == ToLeft)
	{
		xNew = x2 - padding - w1;
		yNew = y2;
	}
	else if (how == ToRight)
	{
		xNew = x2 + w2 + padding;
		yNew = y2;
	}
	else
		LM_RVE(("unrecognized Alignment form"));

	int dx = xNew - x1;
	int dy = yNew - y1;
	manager->groupMove(this, dx, dy);
}



/* ****************************************************************************
*
* isVisible - 
*/
bool QsiBlock::isVisible(void)
{
	if (gItemP != NULL)
		return gItemP->isVisible();
	else if (proxy == NULL)
		return proxy->isVisible();

	LM_E(("Both gItemP and proxy NULL - this can't be!"));
	return NULL;
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
