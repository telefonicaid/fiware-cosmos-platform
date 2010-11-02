/*
 * Delilah.cpp
 *
 *  Created on: Oct 14, 2010
 *      Author: ania
 */

#include <QApplication>
#include "DelilahQt.h"		// Own interface
#include "MainWindow.h"		// MainWindow
#include "Delilah.h"		// ss::Delilah

int DelilahQt::run( int argc , const char * argv[] )
{
	QApplication a( argc ,(char **) argv);
	w = new MainWindow();			// My main window interface
	w->show();
	return a.exec();
}