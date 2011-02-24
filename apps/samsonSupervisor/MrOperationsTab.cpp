/* ****************************************************************************
*
* FILE                     MrOperationsTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QCursor>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QtGui>
#include <QMessageBox>
#include <QGraphicsItem>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*
#include "globals.h"            // connectionMgr, queueMgr, ...

#include "Popup.h"              // Popup
#include "DelilahScene.h"       // DelilahScene
#include "ConnectionMgr.h"      // ConnectionMgr
#include "QueueMgr.h"           // QueueMgr
#include "MrOperationsTab.h"    // Own interface



/* ****************************************************************************
*
* imageV - stores absolute path to images
*/
static char imageV[256];



/* ****************************************************************************
*
* MrOperationsTab::MrOperationsTab - 
*/
MrOperationsTab::MrOperationsTab(const char* name, QWidget *parent) : QWidget(parent)
{
	QVBoxLayout*  mainLayout;
	QHBoxLayout*  topLayout;



	//
	// Creating main widgets
	//
	mainLayout = new QVBoxLayout;
	topLayout  = new QHBoxLayout;
 
	setLayout(mainLayout);



	//
	// Creating Graphics Scene and View
	//
	scene  = new DelilahScene();
	view   = new QGraphicsView(scene);


	QSize         pixmapSize;
	QPixmap*      bg;
	QPixmap*      bg2;

	bg  = new QPixmap(imagePath("background.png", imageV, sizeof(imageV)));
	bg2 = new QPixmap(imagePath("background.png", imageV, sizeof(imageV)));

	sceneLayer0 = scene->addPixmap(*bg);
	sceneLayer1 = scene->addPixmap(*bg2);

	sceneLayer1->setOpacity(0);

	pixmapSize = bg->size();
	scene->setSceneRect(QRectF(0, 0, pixmapSize.width(), pixmapSize.height()));
	view->setMaximumSize(pixmapSize.width(), pixmapSize.height());

	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(view);
}
