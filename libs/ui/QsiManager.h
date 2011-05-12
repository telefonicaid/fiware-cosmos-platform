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

#include "logMsg.h"             // LM_X

#include "QsiFunction.h"        // QsiFunction
#include "QsiCallback.h"        // QsiCallback
#include "QsiBlock.h"           // QsiBlock



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
* QsiManager - 
*/
class QsiManager : public QGraphicsScene
{
	Q_OBJECT

private:
	QGraphicsView*   view;

public:
	int              x;
	int              y;


public:
	void          add(QsiBlock*);
	void          remove(QsiBlock* itemP);
	QsiBlock*     lookup(QGraphicsItem* itemP);
	QsiBlock*     get(int ix);
	int           count();



//
// QsiBlock list
//
private:
	QsiBlock**    item;
	int           itemMax;
	int           items;


//
// Qsi Item Callbacks
//
	QsiCallback** itemCallback;
	int           itemCallbackMax;
	int           itemCallbacks;

	QsiCallback*  itemCallbackLookup(QsiBlock* siP);

public:
	void          siConnect(QsiBlock* siP, QsiFunction func, void* param, bool persistent = false);
	void          siDisconnect(QsiBlock* siP);




//
// Constructors
//
public:
	QsiManager(QVBoxLayout* layout, const char* homeDir, const char* background, int width, int height);
	QsiManager(QObject* parent = 0) { LM_X(1, ("Empty constructor called")); parent = NULL; };
	~QsiManager();

	char*      Home;

	void       init(void);
	void       show(const char* why);



//
// Specific Qsi Item Implementatins
//
	QsiBlock* textAdd(const char*   name, const char* txt,  int _x, int _y);
	QsiBlock* lineAdd(const char*   name,                   int _x, int _y, int _x2, int _y2);
	QsiBlock* buttonAdd(const char* name, const char* txt,  int _x, int _y, int width = -1, int height = -1);
	QsiBlock* inputAdd(const char*  name, const char* txt,  int _x, int _y, int width = -1, int height = -1);
	QsiBlock* imageAdd(const char*  name, const char* path, int _x, int _y, int width = -1, int height = -1);  // Path from 'Home'



//
// Group stuff (will be changed)
//
	void       group(QsiBlock* si1, QsiBlock* si2);
	void       groupMove(QsiBlock* si, int x, int y);



//
// Mouse Events and Actions
//
private:
	void       mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
	void       mouseMoveEvent(QGraphicsSceneMouseEvent*  mouseEvent);


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
};

}

#endif
