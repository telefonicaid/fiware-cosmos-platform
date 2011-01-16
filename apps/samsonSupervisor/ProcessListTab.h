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
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "Endpoint.h"           // Endpoint



/* ****************************************************************************
*
* Class declarations
*/
class Spawner;
class Process;
class Starter;



/* ****************************************************************************
*
* ProcessListTab - 
*/
class ProcessListTab : public QWidget
{
	Q_OBJECT

public:
	ProcessListTab(const char* name, QWidget *parent = 0);

	void      spawnerListCreate(Spawner** spawnerV, int spawners);
	void      processListCreate(Process** processV, int processes);
	void      starterAdd(Starter* starter);
	Starter*  starterLookup(ss::Endpoint* ep);

private slots:
	void connect();
	void start();
	void quit();

private:
	QGridLayout*  mainLayout;

	QPushButton*  connectButton;
	QPushButton*  startButton;
	Starter*      starterV[80];
};

#endif
