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
        
        QBoxLayout* mainLayout;
        QBoxLayout* queuesLayout;
        QBoxLayout* enginesLayout;
    public:
        DelilahMainWindow();
        QWidget* tab1;
        QLabel* enginesText;
        QLabel* queuesText;
        QAction *aboutAction;
        QAction *exitAction;
        QMenu *fileMenu;
        QTabWidget* tabs;
        QGroupBox* queuesBox;
        QGroupBox* enginesBox;
        
    private slots:
        void about();
    };
    
    
}    
    
#endif    
