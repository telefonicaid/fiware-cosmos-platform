#ifndef PROCESS_LIST_TAB_H
#define PROCESS_LIST_TAB_H

/* ****************************************************************************
*
* FILE                     ProcessListTab.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 14 2011
*
*/
#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "Endpoint.h"           // Endpoint
#include "Process.h"            // Process
#include "Starter.h"            // Starter
#include "ProcessConfigView.h"  // ProcessConfigView



/* ****************************************************************************
*
* ProcessListTab - 
*/
class ProcessListTab : public QWidget
{
	Q_OBJECT

public:
	ProcessListTab(const char* name, QWidget *parent = 0);

	void         starterInclude(Starter* starterP);
	void         configShow(Starter* starterP);
	void         processConfigRequest(ss::Process* processP);

private slots:
	void quit();

public:
	QVBoxLayout*        righterLayout;
	ProcessConfigView*  configView;

private:
	QHBoxLayout*        mainLayout;
	QVBoxLayout*        leftLayout;
	QVBoxLayout*        controllerLayout;
	QVBoxLayout*      	workerLayout;
	QVBoxLayout*       	spawnerLayout;

	QVBoxLayout*        rightLayout;
	QGridLayout*        rightGrid;
	void                initialStartersCreate(void);
};

#endif
