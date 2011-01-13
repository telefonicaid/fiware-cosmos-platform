/* ****************************************************************************
*
* FILE                     ButtonActions.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <QWidget>
#include <QPushButton>

#include "logMsg.h"             // LM_*

#include "actions.h"            // help, list, start, ...
#include "ButtonActions.h"      // Own interface



/* ****************************************************************************
*
* ButtonActions::ButtonActions
*/
ButtonActions::ButtonActions(QWidget* window) : QWidget(window)
{
	// Connect Button
	connectButton = new QPushButton("Connect", window);
	connectButton->setGeometry(50, 40, 75, 30);
	connectButton->connect(connectButton, SIGNAL(clicked()), this, SLOT(connect()));

	// Start Button
	startButton = new QPushButton("Start", window);
	startButton->setGeometry(50, 80, 75, 30);
	startButton->connect(startButton, SIGNAL(clicked()), this, SLOT(start()));

	// Quit Button
	quitButton = new QPushButton("Quit", window);
	quitButton->setGeometry(50, 500, 75, 30);
	quitButton->connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));
}



/* ****************************************************************************
*
* ButtonActions::connect
*/
void ButtonActions::connect(void)
{
	connectToSpawners();
}



/* ****************************************************************************
*
* ButtonActions::start
*/
void ButtonActions::start(void)
{
	start();
}



/* ****************************************************************************
*
* ButtonActions::quit
*/
void ButtonActions::quit(void)
{
	LM_X(0, ("Quit button pressed - I quit !"));
}
