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
        au::tables::Table* table = database.getTable("queues");
        QString str;
        if( table != NULL )
        {
            QueueViewer::QueueData data;

            data.name = table->getValue(0, "name");
            data.kvs = table->getValue(0, "block_info/kv_info/kvs");
            data.kvsize = table->getValue(0, "block_info/kv_info/size");
            data.size = table->getValue(0, "block_info/size");
            data.size_locked = table->getValue(0, "block_info/size_locked");
            data.size_on_disk = table->getValue(0, "block_info/size_on_disk");
            data.size_on_memory = table->getValue(0, "block_info/size_on_memory");
                   
            data.kvs_rate = table->getValue(0, "rate_kvs/rate");
            data.kvs_rate_size = table->getValue(0, "rate_kvs/size");
            data.rate = table->getValue(0, "rate_size/rate");
            data.rate_size = table->getValue(0, "rate_size/size");

            mainWindow->queueViewer1->setData(data);
        }
            
    }

/*    void DelilahQt::updateData()
    {
        {
            std::vector<std::string> values;
            au::tables::Table* table = database.getTable("queues");
            QString str;
            if( table)
            {
                std::stringstream tmp;
                for(unsigned int i = 0; i< table->getNumRows(); i++)
                {
                   //for (unsigned int j = 0; j < table->getNumColumns(); j++)
                   //{
                       values.push_back( table->getValue(0, i));
                    //}
                 //std::cout << "Table queues: " << table->str() << std::endl;
                   //emit queuesTextValueChanged(str);
               }
               mainWindow->tableViewer->updateData(values);
            }

        {
            au::tables::Table* table = database.getTable("engines");
            QString str;
            if( !table )
            {
                str = QString("No table engines");
                //std::cout << "No table queues" << std::endl;
                emit enginesTextValueChanged(str);
            }
            else
            {
                std::stringstream tmp;
                tmp << "Table engines with " << table->getNumRows() << " rows";
                str = QString(tmp.str().c_str());
                //std::cout << "Table engines with " << table->getNumRows() << " rows" << std::endl;
                //std::cout << "Table engines: " << table->str() << std::endl;
                emit enginesTextValueChanged(str);
            }
          }
            
        }   
                 
        
    }
  */  
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
        connect(timer, SIGNAL(timeout()), mainWindow, SLOT(update()));
        //connect(this, SIGNAL(enginesTextValueChanged(const QString&)), mainWindow->enginesText, SLOT(setText(const QString&)));
        //connect(this, SIGNAL(queuesTextValueChanged(const QString&)), mainWindow->queuesText, SLOT(setText(const QString&)));
        application.exec();
        
    }
    
}
