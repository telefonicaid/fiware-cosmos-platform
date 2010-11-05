/*
 * DelilahQtApp.cpp
 *
 *  Created on: Nov 4, 2010
 *      Author: ania
 */

#include "DelilahQtApp.h"
#include "MainWindow.h"


DelilahQtApp::DelilahQtApp(int &argc, char ** argv)
	: QApplication(argc, argv)
{
	w = new MainWindow();			// My main window interface
	w->show();
}

