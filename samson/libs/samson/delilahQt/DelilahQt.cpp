/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/* ****************************************************************************
 *
 * FILE                     DelilahQt.cpp
 *
 * DESCRIPTION			   QT terminal for delilah
 *
 */

#include "DelilahQt.h" // Own interface
//#include "DelilahMainWindow.h"
#include "WorkerViewer.h"

#include "samson/client/SamsonClient.h"

#include <time.h>

#include <QTimer>
#include <QtGui>

#include <sstream>

namespace samson
{	
    void DelilahQt::updateData()
    {
        //au::tables::Table* table2 = database.getTable("workers");
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
   }

    DelilahQt::DelilahQt(  )
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
        mainWindow->setWindowTitle("Delilah Graph");
        
        //updateData();
        //mainWindow->tableViewer->setData(names, values);
        mainWindow->show();

        connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
        connect(mainWindow, SIGNAL(requestUpdate()), this, SLOT(updateData()));
        connect(mainWindow, SIGNAL(updateConnection(ConnectQueueParameters)), this, SLOT(onConnectionUpdate(ConnectQueueParameters)));
        connect(timer, SIGNAL(timeout()), mainWindow, SLOT(update()));
        //connect(this, SIGNAL(enginesTextValueChanged(const QString&)), mainWindow->enginesText, SLOT(setText(const QString&)));
        //connect(this, SIGNAL(queuesTextValueChanged(const QString&)), mainWindow->queuesText, SLOT(setText(const QString&)));
        application.exec();
       
    }
    
    void DelilahQt::receive_buffer_from_queue( std::string queue , engine::Buffer* buffer )
    {
        size_t size = buffer->getSize();
        LM_M(("Received buffer with size %s from queue %s" , au::str(size,"B").c_str() , queue.c_str() )  );
        if(connectedQueues.find(queue) != connectedQueues.end())
        {
            time_t rawtime;
            struct tm * timeinfo;

            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            std::string timeString = std::string(asctime (timeinfo));

            std::stringstream headerString;
            //line << timeString << "Received buffer with size " << au::str(size,"B") << " from queue " << queue;
            // Show the first line or key-value
            SamsonClientBlock samson_client_block( buffer ); 
            
            headerString << "====================================================================\n";
            headerString << au::str("Received stream data for queue %s\n" , queue.c_str() ); 
            headerString << samson_client_block.get_header_content();
            headerString << "====================================================================\n";
            std::string dataString = samson_client_block.get_content( 5 );

            mainWindow->updateQueuesFeed(queue, headerString.str(), dataString);
        }
        
    }

    void DelilahQt::onConnectionUpdate(ConnectQueueParameters params)
    {
        std::string command;
        if(params.connected)
        {   
            command = au::str("connect_to_queue %s" , params.queueName.c_str() );
            connectedQueues[params.queueName] = true;
            if(params.newData) command += " -new";
            if(params.clearPopped) command += " -clean";
        }
        else
        {
            command = au::str("disconnect_from_queue %s" , params.queueName.c_str() );
            connectedQueues.erase(params.queueName);
        }
        sendWorkerCommand( command , NULL );

    }
}
