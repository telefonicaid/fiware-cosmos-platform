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



namespace Qsi
{



class QsiManager;



typedef union QtWidget
{
	void*         vP;
	QPushButton*  button;
	QLabel*       label;
	QLineEdit*    lineEdit;
	QPixmap*      pixmap;
} QtWidget;


typedef enum QsiBlockType
{
	SimpleText,
	Image,
	Label,
	Button,
	Input,
	Line
} QsiBlockType;



/* ****************************************************************************
*
* QsiBlock - 
*/
class QsiBlock : public QsiBase
{
public:
	typedef enum Alignment
	{
		Over,
		Under,
		ToLeft,
		ToRight
	} Alignment;

	QsiManager*            manager;
	QsiBlockType           type;
	QGraphicsItem*         gItemP;
	QtWidget               w;
	QGraphicsProxyWidget*  proxy;
	QsiBlock*              groupPrev;
	QsiBlock*              groupNext;
	QFont                  font;
	QPixmap*               pixmap;

private:
	bool                   movable;
	bool                   expanded;

public:
	QsiBlock(QsiManager* _manager, QsiBlockType type, const char* name, const char* txt, int _x, int _y, int width = -1, int height = -1);
	~QsiBlock();

	const char*  typeName(void);

	void         setPos(int x, int y);
	void         setGroupPos(int x, int y);
	void         move(int _x, int _y);
	void         move(void);

	void         hide(void);
	void         show(void);
	bool         isVisible(void);

	void         expand(void);
	void         compress(void);
	bool         isExpanded(void);

	void         setSize(int width, int height);
	void         scale(int width, int height);
	void         scale(int percentage);

	bool         menu;
	void         menuAdd(const char* title, QsiFunction func, void* param);
	void         menuClear(void);

	char*        menuTitle[10];
	QsiFunction  menuFunc[10];
	void*        menuParam[10];

	void         setFont(QFont* font);
	void         setFontSize(int size);
	void         setFontColor(int color);

	void         setMovable(bool _movable);
	bool         getMovable(void);

	void         geometryGet(int* xP, int* yP, int* widthP, int* heightP, bool force = false);
	void         groupGeometryGet(int* xP, int* yP, int* widthP, int* heightP);

	QsiBlock*    groupLeader(void);	
	bool         inGroup(void);
	void         align(Alignment how, QsiBlock* s2, int padding);

	const char*  getText(void);
	void         setText(const char* txt);

	void         setFont(QFont font);
	void         setColor(int r, int g, int b, int a = 255);
	void         setBold(bool onoff);
	void         setItalic(bool onoff);
};

}

#endif
