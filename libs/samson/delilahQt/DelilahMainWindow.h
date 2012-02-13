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

#include "TableViewer.h"

namespace samson {
	
	/**
	 Widget class for the DelilahQT GUI
	 */
    class DelilahMainWindow : public QMainWindow
    {
        Q_OBJECT
        
        QBoxLayout* mainLayout;
        /*QBoxLayout* queuesLayout;
        QBoxLayout* enginesLayout;*/

        QTabWidget* tabs;
        QWidget* tab1;
        QAction *aboutAction;
        QAction *exitAction;
        QMenu *fileMenu;

    public:
    
        DelilahMainWindow();
        TableViewer* tableViewer;
        /*QLabel* enginesText;
        QLabel* queuesText;
        QGroupBox* queuesBox;
        QGroupBox* enginesBox;*/
        
    private slots:
        void about();
    };
    
    
}    
    
#endif    
