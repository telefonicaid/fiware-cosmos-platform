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

#include "QsiFunction.h"        // QsiFunction
#include "QsiBase.h"            // QsiBase
#include "QsiAlignment.h"       // QsiAlignment



namespace Qsi
{



class QsiManager;



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
* QsiBlock - 
*/
class QsiBlock : public QsiBase
{
public:
	QsiManager*            manager;  // Hope this will not be necessary ...
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
	QsiBlock(QsiManager* manager, QsiBox* owner, QsiType type, const char* name, const char* txt, int x, int y, int width = -1, int height = -1);
	~QsiBlock();

	void         geometry(int* xP, int* yP, int* widthP, int* heightP);
	void         moveRelative(int x, int y);
	void         moveAbsolute(int x, int y);
	void         align(Alignment::Type type, QsiBase* master, int margin);
	void         hide(void);
	void         hideOthers();
	void         show(void);
	void         showOthers();
	bool         isVisible(void);
	bool         isExpanded(void);

	bool         menu;
	void         menuAdd(const char* title, QsiFunction func, void* param);
	void         menuClear(void);

	char*        menuTitle[10];
	QsiFunction  menuFunc[10];
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
