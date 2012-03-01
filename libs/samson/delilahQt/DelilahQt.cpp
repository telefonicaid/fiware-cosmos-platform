/* ****************************************************************************
 *
 * FILE                     DelilahQt.cpp
 *
 * DESCRIPTION			   QT terminal for delilah
 *
 */

#include "DelilahQt.h" // Own interface
#include "DelilahMainWindow.h"
#include "WorkerViewer.h"

#include <QTimer>
#include <QtGui>

#include <sstream>

namespace samson
{	
    void DelilahQt::updateData()
    {
        //au::tables::Table* table2 = database.getTable("workers");
        //if( table2 ) std::cout << "Engines read" << std::endl;
        QString str;
        std::vector<QueueData*> queuesData;
        std::vector<WorkerData*> workersData;
        au::tables::Table* table = database.getTable("queues");
        if( table )
        {
            //Store all queues in the table to send to DelilahMainWindow for processing
            for(unsigned int i = 0; i< table->getNumRows(); i++)
            {
                QueueData* data = new QueueData;
                data->name = table->getValue(i, "name");
     //std::cout << "Name: " << data.name << std::endl;
                data->kvs  = table->getValue(i, "#kvs");
                data->size = table->getValue(i, "size");
                data->key = table->getValue(i, "key");
                data->value = table->getValue(i, "value");
                data->total_kvs = table->getValue(i, "Total #kvs");
                data->total_size = table->getValue(i, "Total size");
                data->kvs_s = table->getValue(i, "#kvs/s");
                data->bytes_s = table->getValue(i, "Bytes/s");
                data->blocks = table->getValue(i, "#Blocs");
                data->rate_size = table->getValue(i, "Size");
                data->on_memory = table->getValue(i, "on Memory");
                data->on_disk = table->getValue(i, "on Disk");
                data->locked = table->getValue(i, "Locked");
                data->time_from = table->getValue(i, "Time from");
                data->time_to = table->getValue(i, "Time to");
                queuesData.push_back(data);
                
            }
        }
       
        table = database.getTable("workers");
        if( table )
        {
           //Store all workers in the table to send to DelilahMainWindow for processing
            for(unsigned int i = 0; i< table->getNumRows(); i++)
            {
                WorkerData* data = new WorkerData;
                data->worker_id = table->getValue(i, "worker_id");
                data->type = table->getValue(i, "Type");
                data->mem_used = table->getValue(i, "Mem used");
                data->mem_total = table->getValue(i, "Mem total");
                data->cores_used = table->getValue(i, "Cores used");
                data->cores_total = table->getValue(i, "Cores total");
                data->disk_ops = table->getValue(i, "#Disk ops");
                data->disk_in_rate = table->getValue(i, "Disk in B/s");
                data->disk_out_rate = table->getValue(i, "Disk out B/s");
                data->net_in = table->getValue(i, "Net in B/s");
                data->net_out = table->getValue(i, "Net out B/s");
                workersData.push_back(data);
            }
        }
        mainWindow->updateData(queuesData, workersData);
    //std::cout << "DelilahQt::updateData()- queues: " << queuesData.size() << std::endl;
   }

    DelilahQt::DelilahQt( NetworkInterface *network ) : Delilah( network )
    {
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
        connect(mainWindow, SIGNAL(connectedQueue(std::string)), this, SLOT(onConnectedQueue(std::string)));
        connect(mainWindow, SIGNAL(disconnectedQueue(std::string)), this, SLOT(onDisconnectedQueue(std::string)));
        connect(timer, SIGNAL(timeout()), mainWindow, SLOT(update()));
        //connect(this, SIGNAL(enginesTextValueChanged(const QString&)), mainWindow->enginesText, SLOT(setText(const QString&)));
        //connect(this, SIGNAL(queuesTextValueChanged(const QString&)), mainWindow->queuesText, SLOT(setText(const QString&)));
        application.exec();
       
    }
    
    void DelilahQt::process_stream_out_packet( std::string queue , engine::Buffer* buffer )
    {
        size_t size = buffer->getSize();
        LM_M(("Received buffer with size %s from queue %s" , au::str(size,"B").c_str() , queue.c_str() )  );
   //std::cout << "Received buffer with size " << au::str(size,"B") << " from queue " << queue << std::endl;
        if(connectedQueues.find(queue) != connectedQueues.end())
        {
            std::stringstream line;
            line << "Received buffer with size " << au::str(size,"B") << " from queue " << queue;
            mainWindow->updateQueuesFeed(queue, line.str());
        }
    }

    void DelilahQt::onConnectedQueue(std::string name)
    {
        connectedQueues[name] = true;
        sendWorkerCommand( au::str("connect_to_queue %s" , name.c_str() ) , NULL );

    }
    void DelilahQt::onDisconnectedQueue(std::string name)
    {
        connectedQueues.erase(name);
        sendWorkerCommand( au::str("disconnect_from_queue %s" , name.c_str() ) , NULL );
    }
}
