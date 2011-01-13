#ifndef BUTTON_ACTIONS_H
#define BUTTON_ACTIONS_H

/* ****************************************************************************
*
* FILE                     ButtonActions.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <QObject>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "Spawner.h"            // Spawner
#include "Process.h"            // Process



/* ****************************************************************************
*
* ButtonActions - 
*/
class ButtonActions : public QWidget
{
	Q_OBJECT

public:
	ButtonActions(QWidget* parent = 0);
	void spawnerListCreate(Spawner** spawnerV, int spawners);
	void processListCreate(Process** processV, int processes);

private slots:
    void connect();
    void start();
    void quit();

private:
	QWidget*      win;
	QHBoxLayout*  layout;

	QVBoxLayout*  buttonLayout;
	QVBoxLayout*  spawnerListLayout;
	QVBoxLayout*  processListLayout;

	QPushButton*  quitButton;
	QPushButton*  connectButton;
	QPushButton*  startButton;
};

#endif
