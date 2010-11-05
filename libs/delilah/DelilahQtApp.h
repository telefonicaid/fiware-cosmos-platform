/*
 * DelilahQtApp.h
 *
 *  Created on: Nov 4, 2010
 *      Author: ania
 */

#ifndef DELILAHQTAPP_H_
#define DELILAHQTAPP_H_

#include <QApplication>

//#include "MainWindow.h"
class MainWindow;

class DelilahQtApp : public QApplication
{
public:
	DelilahQtApp(int &argc, char ** argv);
	~DelilahQtApp() {};

public:
	MainWindow* w;
};


#endif /* DELILAHQTAPP_H_ */
