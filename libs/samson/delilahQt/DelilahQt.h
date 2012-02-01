#ifndef _H_Delilah_QT
#define _H_Delilah_QT

/* ****************************************************************************
*
* FILE                     DelilahQT.h
*
* DESCRIPTION			   QT terminal for delilah
*
*/


#include "logMsg/logMsg.h"				
#include "samson/delilah/Delilah.h"			// samson::Delilah

#include <QObject>
#include <QtGui>

namespace samson {
	
	/**
	 Widget class for the DelilahQT GUI
	 */
    class DelilahMainWindow : public QWidget
    {
        Q_OBJECT
        
        QBoxLayout* layout;
    public:
        QLabel* enginesText;
        QLabel* queuesText;
        DelilahMainWindow(QWidget* parent = 0);
    };
    
	/**
	 Main class for the DelilahQT program
	 */
	
	class DelilahQt : public QObject, public Delilah
	{
		Q_OBJECT
		
	public:
		
		DelilahQt( NetworkInterface *network );        
        
        // Main run command ( it should be blocking )
        void run();
    signals:
        void enginesTextValueChanged(const QString& s);
        void queuesTextValueChanged(const QString& s);

    public slots:
        void updateData();

        
    };

}


#endif
