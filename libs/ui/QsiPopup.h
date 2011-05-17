#ifndef QSI_POPUP_H
#define QSI_POPUP_H

/* ****************************************************************************
*
* FILE                     QsiPopup.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include "QsiBox.h"             // Box



namespace Qsi
{



/* ****************************************************************************
*
* - 
*/
class Manager;



/* ****************************************************************************
*
* Popup - 
*/
class Popup
{
private:

public:
	Popup(Manager* manager, const char* _title, const char* message);
};

}

#endif
