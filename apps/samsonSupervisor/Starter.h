#ifndef STARTER_H
#define STARTER_H

/* ****************************************************************************
*
* FILE                     Starter.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <QObject>
#include <QBoxLayout>
#include <QCheckBox>
#include <QToolButton>
#include <QPushButton>
#include <QGridLayout>

#include "Process.h"            // Process



/* ****************************************************************************
*
* Starter - 
*/
class Starter : public QWidget
{
	Q_OBJECT

public:
	Starter(Process* processP);
	Starter(Process* processP, SpawnInfo* siP);
	
	void            check(const char* reason);
	void            qtInit(QVBoxLayout* spawnerLayout, QVBoxLayout* workerLayout, QVBoxLayout* controllerLayout);
	void            processStart(void);
	void            processKill(void);
	void            processConnect(void);

	Process*        process;

	QPushButton*    startButton;
	QPushButton*    logButton;
	QPushButton*    nameButton;

private slots:
	void startClicked();
	void connectClicked();
	void logClicked();
	void nameClicked();
};

#endif
