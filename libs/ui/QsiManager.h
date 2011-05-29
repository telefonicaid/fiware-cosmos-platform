#ifndef QSI_MANAGER_H
#define QSI_MANAGER_H

/* ****************************************************************************
*
* FILE                     QsiManager.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 09 2011
*
*/
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QAction>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsSceneWheelEvent>
#include <QWheelEvent>

#include "QsiFunction.h"        // Function
#include "QsiCallback.h"        // Callback
#include "QsiBlock.h"           // Block



namespace Qsi
{



/* ****************************************************************************
*
* Status - 
*/
typedef enum Status
{
	OK = 1
} Status;



/* ****************************************************************************
*
* QSI_MENU_ACTIONS - maximum 10 items in pull-down menu (at least for now ...)
*/
#define QSI_MENU_ACTIONS 10



/* ****************************************************************************
*
* Classes
*/
class Box;



/* ****************************************************************************
*
* Manager - 
*/
class Manager : public QGraphicsScene
{
	Q_OBJECT

private:
	QGraphicsView*  view;
	Box*            modal;

public:
	QGraphicsItem*  sceneLayer0;   // Connection lines will be placed between these two layers
	QGraphicsItem*  sceneLayer1;   // Connection lines will be placed between these two layers

	int             x;
	int             y;
	int             totalWidth;
	int             totalHeight;
    int             winWidth;
	int             winHeight;

public:
	void       grab(Base* base);
	void       ungrab(Box* box);

	void       add(Block*);
	void       remove(Block* itemP);
	Block*     lookup(QGraphicsItem* itemP);
	Block*     get(int ix);
	int        count();



//
// Block list
//
private:
	Block**    item;
	int        itemMax;
	int        items;


//
// Qsi Item Callbacks
//
	Callback** itemCallback;
	int        itemCallbackMax;
	int        itemCallbacks;

	Callback*  itemCallbackLookup(Block* qbP);

public:
	void       siConnect(Block* qbP, Function func, const void* param, bool persistent = false);
	void       siDisconnect(Block* qbP);



//
// Constructors
//
public:
	Manager(QVBoxLayout* layout, const char* homeDir, const char* background, int width, int height, int _winWidth, int _winHeight);
	Manager(QObject* parent = 0);
	~Manager();

	char*    Home;
	Box*     box;
	void     init(void);


//
// Mouse Events and Actions
//
private:
	void       mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
	void       mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
	void       mouseMoveEvent(QGraphicsSceneMouseEvent*  mouseEvent);
	void       wheelEvent(QGraphicsSceneWheelEvent* event);
	void       wheelEvent(QWheelEvent* event);

public:
	QAction*   menuAction[QSI_MENU_ACTIONS];

public slots:
	void       contextMenuEvent(QGraphicsSceneContextMenuEvent* contextMenuEvent);
	void       menuActionFunc0();
	void       menuActionFunc1();
	void       menuActionFunc2();
	void       menuActionFunc3();
	void       menuActionFunc4();
	void       menuActionFunc5();
	void       menuActionFunc6();
	void       menuActionFunc7();
	void       menuActionFunc8();
	void       menuActionFunc9();

//
// Menu for background
//
public:
	void         menuAdd(const char* title, Function func, void* param);
	void         menuClear(void);
	char*        menuTitle[QSI_MENU_ACTIONS];
	Function     menuFunc[QSI_MENU_ACTIONS];
	void*        menuParam[QSI_MENU_ACTIONS];
};

}

#endif
