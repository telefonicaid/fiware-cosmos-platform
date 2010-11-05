/*
 * Delilah.cpp
 *
 *  Created on: Oct 14, 2010
 *      Author: ania
 */

#include "DelilahQt.h"		// Own interface
#include "Delilah.h"		// ss::Delilah
#include "DelilahQtApp.h"


int DelilahQt::run( int argc , const char * argv[] )
{
	app = new DelilahQtApp(argc, (char**) argv);
	return app->exec();
}
