/* ****************************************************************************
 *
 * FILE                     DelilahQt.cpp
 *
 * DESCRIPTION			   QT terminal for delilah
 *
 */

#include "DelilahQt.h" // Own interface
#include "DelilahMainWindow.h"

#include <QTimer>
#include <QtGui>

#include <sstream>

namespace samson
{	
    void DelilahQt::updateData()
    {
        //au::tables::Table* table2 = database.getTable("workers");
        //if( table2 ) std::cout << "Engines read" << std::endl;
        au::tables::Table* table = database.getTable("queues");
        QString str;
        std::vector<QueueData> queuesData;
        if( table )
        {
            //Send all queues in the table to the DelilahMainWindow for processing
            for(unsigned int i = 0; i< table->getNumRows(); i++)
            {
                QueueData data;
                data.name = table->getValue(i, "name");
                data.kvs  = table->getValue(i, "#kvs");
                data.size = table->getValue(i, "size");
                data.key = table->getValue(i, "key");
                data.value = table->getValue(i, "value");
                data.total_kvs = table->getValue(i, "Total #kvs");
                data.total_size = table->getValue(i, "Total size");
                data.kvs_s = table->getValue(i, "#kvs/s");
                data.bytes_s = table->getValue(i, "Bytes/s");
                data.blocks = table->getValue(i, "#Blocs");
                data.rate_size = table->getValue(i, "Size");
                data.on_memory = table->getValue(i, "on Memory");
                data.on_disk = table->getValue(i, "on Disk");
                data.locked = table->getValue(i, "Locked");
                data.time_from = table->getValue(i, "Time from");
                data.time_to = table->getValue(i, "Time to");
                queuesData.push_back(data);
                
            }
        }
        mainWindow->updateData(queuesData);
    }

    DelilahQt::DelilahQt( NetworkInterface *network ) : Delilah( network )
    {
        // Ask continuously workers about queues to monitor them
        automatic_update = true;
        
    }
    
    void DelilahQt::run()
    {
        
        int argn = 0;
        QApplication application(argn, NULL);
        application.setStyle(new QPlastiqueStyle);
        
        QTimer* timer =  new QTimer();
        timer->start(1000);
            
        mainWindow = new DelilahMainWindow;
        
        //updateData();
        //mainWindow->tableViewer->setData(names, values);
        mainWindow->show();

        connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
        connect(mainWindow, SIGNAL(requestUpdate()), this, SLOT(updateData()));
        connect(timer, SIGNAL(timeout()), mainWindow, SLOT(update()));
        //connect(this, SIGNAL(enginesTextValueChanged(const QString&)), mainWindow->enginesText, SLOT(setText(const QString&)));
        //connect(this, SIGNAL(queuesTextValueChanged(const QString&)), mainWindow->queuesText, SLOT(setText(const QString&)));
        application.exec();
       
    }
    
}
