#ifndef TAB_MANAGER_H
#define TAB_MANAGER_H

/* ****************************************************************************
*
* FILE                     TabManager.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <QDialog>
#include <QTimerEvent>

#include "ProcessListTab.h"     // ProcessListTab
#include "LogTab.h"             // LogTab
#include "DelilahTab.h"         // DelilahTab



/* ****************************************************************************
*
* Class declarations
*/
class QTabWidget;



/* ****************************************************************************
*
* TabManager - 
*/
class TabManager : public QWidget
{
	Q_OBJECT

public:
	TabManager(QWidget* window, QWidget *parent = 0);

	ProcessListTab*  processListTab;
	LogTab*          logTab;
	DelilahTab*      delilahTab;

protected:
	void timerEvent(QTimerEvent* e);

private:
	QTabWidget*       tabWidget;
};

#endif
