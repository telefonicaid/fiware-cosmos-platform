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
 * FILE                     DelilahMainWindow.cpp
 *
 * DESCRIPTION			   Main widget for delilahQt
 *
 */

#include "DelilahMainWindow.h" // Own interface

#include "au/string.h"

#include <QTimer>
#include <QtGui>

#include <iostream>
#include <sstream>

                        
namespace samson
{	

    ConfigurableTabWidget::ConfigurableTabWidget(QWidget * parent ):QTabWidget(parent)
    {
    }
    
    void ConfigurableTabWidget::enableClosing(int index, bool enable)
    {
        if(enable)
        {
            tabBar()->tabButton(index, QTabBar::RightSide)->show();
        }
        else
        {
            tabBar()->tabButton(index, QTabBar::RightSide)->hide();
        }
    }
    
    DelilahMainWindow::DelilahMainWindow()
    {
        tabs = new ConfigurableTabWidget(this);
        tabs->setTabsClosable(true);
        connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosed(int)));
        setCentralWidget(tabs);
        
        queuesTab = new QueueContainer;
        workersTab = new WorkerContainer;
        
        int tabposition = tabs->addTab(queuesTab, tr("Queues"));
        tabs->enableClosing(tabposition, false);
        
        tabposition = tabs->addTab(workersTab, tr("Workers"));
        tabs->enableClosing(tabposition, false);

        exitAction = new QAction(tr("E&xit"), this);
        aboutAction = new QAction(tr("A&bout"), this);
        fileMenu = menuBar()->addMenu(tr("&File"));
        fileMenu->addAction(aboutAction);
        fileMenu->addAction(exitAction);
                
        QObject::connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

        connect(queuesTab, SIGNAL(queueDetailsClicked(QueueViewer*)), this, SLOT(onQueueDetailsClicked(QueueViewer*)));
        connect(queuesTab, SIGNAL(queueHasChanged(QueueViewer*, QueueData*)), this, SLOT(onQueueHasChanged(QueueViewer*, QueueData*)));
        connect(queuesTab, SIGNAL(queueDeleted(std::string)), this, SLOT(onQueueDeleted(std::string)));

        setMinimumSize(800,600); 
    }
    
    void DelilahMainWindow::about()
    {
        QMessageBox::about( this, tr("Samson"), tr("Samson version 0.7.0") );
    }
    
    void DelilahMainWindow::updateData(std::vector<QueueData*> queuesData, std::vector<WorkerData*> workersData)
    {
        //Update queues changes
        queuesTab->setData(queuesData);
        //Update worker changes
        workersTab->setData(workersData);
            
    }
    
    ExtQueueViewer* DelilahMainWindow::findQueueTab(std::string name)
    {
        bool found = false;
        ExtQueueViewer* queueTmp = NULL;
        for(unsigned int i = 0; i< tabbedQueues.size() && !found; i++)
        {
            if (tabbedQueues[i]->title == name)
            {
                found = true;
                queueTmp = tabbedQueues[i];
            }
        }
        
        return queueTmp;
        
    }    
    
    void DelilahMainWindow::onQueueDetailsClicked(QueueViewer* queue)
    {
        //Check if the queue already has a tab
        ExtQueueViewer* tabbedQueue = findQueueTab(queue->title);
        if(tabbedQueue)
        {
            tabs->setCurrentWidget(tabbedQueue);
        }
        else
        {
            //There is not an existing tab for the queue. Create one.
            tabbedQueue =  new ExtQueueViewer(queue->title, this);
            tabs->addTab(tabbedQueue, QString(queue->title.c_str()));
            tabbedQueues.push_back(tabbedQueue);
            emit requestUpdate();
            tabbedQueue->setData(&(queue->data));
            tabs->setCurrentWidget(tabbedQueue);
            connect(tabbedQueue, SIGNAL(updateConnection(ConnectQueueParameters)), this, SLOT(onConnectionUpdate(ConnectQueueParameters))); 
        }
    }
    
    void DelilahMainWindow::tabClosed(int index)
    {
        ExtQueueViewer* removedQueue = dynamic_cast<ExtQueueViewer*>(tabs->widget(index));
        if(removedQueue)
        {
            removedQueue->disconnect();
            //Find the queue in the list and remove it
            bool found;
            for (std::vector<ExtQueueViewer*>::iterator queuesIter = tabbedQueues.begin();
            queuesIter < tabbedQueues.end() && !found; queuesIter++)
            {
                if((*queuesIter)->title == removedQueue->title)
                {
                    found = true;
                    tabbedQueues.erase(queuesIter);
                    delete *queuesIter;
                }
            }
            tabs->removeTab(index);
        }
    }

    void DelilahMainWindow::onQueueHasChanged(QueueViewer* queue, QueueData* data)
    {
        ExtQueueViewer* tabbedQueue = findQueueTab(queue->title);
        if (tabbedQueue)
        {
            tabbedQueue->setData(data);
        }
    }

    void DelilahMainWindow::onQueueDeleted(std::string queueName)
    {
        //if there is a tab for this queue, remove it
        ExtQueueViewer* queueTmp = findQueueTab(queueName);
        if(queueTmp)
        {
            queueTmp->disconnect();
            tabs->removeTab(tabs->indexOf(queueTmp));
        }
    }
    
    void DelilahMainWindow::onConnectionUpdate(ConnectQueueParameters params)
    {
        emit(updateConnection(params));
    }

    void DelilahMainWindow::updateQueuesFeed(std::string queuename, std::string header, std::string data)
    {
        ExtQueueViewer* queue = findQueueTab(queuename);
        if(queue) 
        {
            queue->updateHeader(header);
            queue->updateFeed(data);
        }
    }


}
