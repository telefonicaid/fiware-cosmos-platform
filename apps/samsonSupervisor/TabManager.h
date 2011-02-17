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
#include "MrOperationsTab.h"    // MrOperationsTab
#include "LogTab.h"             // LogTab
#include "DelilahRawTab.h"      // DelilahRawTab
#include "ConfigTab.h"          // ConfigTab
#include "OldMrOperationsTab.h" // OldMrOperationsTab



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

	ProcessListTab*      processListTab;
	MrOperationsTab*     mrOperationsTab;
	LogTab*              logTab;
	DelilahRawTab*       delilahRawTab;
	ConfigTab*           configTab;
	OldMrOperationsTab*  delilahOldTab;

protected:
	void timerEvent(QTimerEvent* e);

	int  logSocket;
	void logReceiverInit(unsigned short port);
	void logReceive(void);

private:
	QTabWidget*       tabWidget;
};

#endif
