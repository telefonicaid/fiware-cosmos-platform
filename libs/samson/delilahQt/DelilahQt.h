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

#include "DelilahMainWindow.h"

#include <QObject>
#include <QtGui>

namespace samson {
	
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
        void setData();
        void updateData();
        
    private:
        DelilahMainWindow* mainWindow;

        
    };

}


#endif
