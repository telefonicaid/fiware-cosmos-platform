#ifndef QSI_BLOCK_H
#define QSI_BLOCK_H

/* ****************************************************************************
*
* FILE                     QsiBlock.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 10 2011
*
*/
#include <QObject>
#include <QGraphicsItem>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFont>

#include "QsiFunction.h"        // Function
#include "QsiAlignment.h"       // Alignment
#include "QsiBase.h"            // Base



namespace Qsi
{



class Manager;



/* ****************************************************************************
*
* ProxiedWidget - 
*/
typedef union ProxiedWidget
{
	void*         vP;
	QPushButton*  button;
	QLabel*       label;
	QLineEdit*    lineEdit;
	QPixmap*      pixmap;
} ProxiedWidget;



/* ****************************************************************************
*
* Block - 
*/
class Block : public Base
{
public:
	Manager*               manager;  // Hope this will not be necessary ...
	QGraphicsItem*         gItemP;
	ProxiedWidget          w;
	QGraphicsProxyWidget*  proxy;
	QFont                  font;
	QPixmap*               pixmap;

private:
	bool                   movable;
	bool                   boxMove;
	bool                   expanded;

public:
	Block(Manager* manager, Box* owner, Type type, const char* name, const char* txt, int x, int y, int width = -1, int height = -1);
	~Block();

	int          geometry(int* xP, int* yP, int* widthP, int* heightP);
	void         moveRelative(int x, int y);
	void         moveAbsolute(int x, int y);
	void         align(Alignment::Type type, Base* master, int margin);
	void         hide(void);
	void         hideOthers();
	void         show(void);
	void         showOthers();
	bool         isVisible(void);
	bool         isExpanded(void);

	bool         menu;
	void         menuAdd(const char* title, Function func, void* param);
	void         menuClear(void);

	char*        menuTitle[10];
	Function     menuFunc[10];
	void*        menuParam[10];

	void         setMovable(bool movable);
	bool         getMovable(void);

	void         setBoxMove(bool boxMove);
	bool         getBoxMove(void);

	void         setSize(int width, int height);
	void         scale(int width, int height);
	void         scale(int percentage);

	const char*  getText(void);
	void         setText(const char* txt);

	void         setFont(QFont font);
	void         setColor(int r, int g, int b, int a = 255);
	void         setBold(bool onoff);
	void         setItalic(bool onoff);
};

}

#endif
