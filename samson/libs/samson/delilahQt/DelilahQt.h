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
#include "engine/Buffer.h"
#include "samson/delilah/Delilah.h"			// samson::Delilah

#include "DelilahMainWindow.h"
#include "WorkerViewer.h"

#include <QObject>
#include <QtGui>

#include <map>

namespace samson {
	
	/**
	 Main class for the DelilahQT program
	 */
	
	class DelilahQt : public QObject, public Delilah
	{
		Q_OBJECT
		
	public:
		
		DelilahQt( );     
        
        // Main run command ( it should be blocking )
        void run();
    signals:
        void enginesTextValueChanged(const QString& s);
        void queuesTextValueChanged(const QString& s);

    public slots:
        //void setData();
        void updateData();
        void onConnectionUpdate(ConnectQueueParameters params);
        //void onConnectedQueue(std::string name);
        //void onDisconnectedQueue(std::string name);
        
    private:
        DelilahMainWindow* mainWindow;
        //store the name list in a map for quick access using its hash table
        std::map<std::string, bool> connectedQueues;

        
    public:
        
        // Overloaded functions from Delilah
        virtual void receive_buffer_from_queue( std::string queue , engine::Buffer* buffer );
        
        
        
    };

}


#endif
