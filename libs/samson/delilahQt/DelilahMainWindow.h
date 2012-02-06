#ifndef _H_DelilahMainWindow
#define _H_DelilahMainWindow

/* ****************************************************************************
*
* FILE                     DelilahQT.h
*
* DESCRIPTION			   Main widget for delilahQt
*
*/


#include "logMsg/logMsg.h"				

#include <QObject>
#include <QtGui>

namespace samson {
	
	/**
	 Widget class for the DelilahQT GUI
	 */
    class DelilahMainWindow : public QMainWindow
    {
        Q_OBJECT
        
        QBoxLayout* layout;
    public:
        QLabel* enginesText;
        QLabel* queuesText;
        DelilahMainWindow();
        QAction *aboutAction;
        QAction *exitAction;
        QMenu *fileMenu;
        
    private slots:
        void about();
    };
    
    
}    
    
#endif    
