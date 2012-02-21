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
#include <fnmatch.h>


namespace samson
{	
    DelilahMainWindow::DelilahMainWindow()
    {
        tabs = new QTabWidget(this);
        setCentralWidget(tabs);
        
        queuesTab = new QWidget();

        mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, queuesTab);
 
        //queueViewer1 = new QueueViewer(queuesTab);
        //mainLayout->addWidget(queueViewer1);
                
        tabs->addTab(queuesTab, tr("Queues"));
        
        exitAction = new QAction(tr("E&xit"), this);
        aboutAction = new QAction(tr("A&bout"), this);
        fileMenu = menuBar()->addMenu(tr("&File"));
        fileMenu->addAction(aboutAction);
        fileMenu->addAction(exitAction);
        
        //Initialise groupboxes
        inputBox = new QGroupBox("Input Queues", this);
        outputBox = new QGroupBox("Output Queues", this);
        totalBox = new QGroupBox("Totals", this);
        mainLayout->addWidget(inputBox);
        mainLayout->addWidget(outputBox);
        mainLayout->addWidget(totalBox);
        mainLayout->addStretch();
        
        noInputLabel = new QLabel("No input Queues", inputBox);
        noOutputLabel = new QLabel("No output Queues", outputBox);
        
        inputLayout = new QGridLayout(inputBox);
        inputLayout->addWidget(noInputLabel, 0, 0);
        inputBox->setLayout(inputLayout);
        
        outputLayout = new QGridLayout(outputBox);
        outputLayout->addWidget(noOutputLabel, 0, 0);
        outputBox->setLayout(outputLayout);

        totalLayout = new QGridLayout(totalBox);
        totalQueues = new QueueViewer("Total", totalBox);
        totalQueues ->setHiddenButton(true);
        totalQueues->setLayout(totalLayout, 0);
        totalBox->setLayout(totalLayout);
                
        QObject::connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
        
        setMinimumSize(600,600); 
    }
    
    void DelilahMainWindow::about()
    {
        QMessageBox::about( this, tr("Samson"), tr("Samson version 0.6.1") );
    }
    
    void DelilahMainWindow::updateData(std::vector<QueueData>& queuesData)
    {
        bool any_change = false;
        size_t totalKvs = 0;
        size_t totalSize = 0;
        size_t totalRate = 0;
        //For each queue, check if it has already a widget defined. otherwise create it
        for(unsigned int i = 0; i < queuesData.size(); i++)
        {
            //Increment totals
            totalKvs += atol(queuesData[i].kvs.c_str());
            totalSize += atol(queuesData[i].size.c_str());
            totalRate += atol(queuesData[i].rate.c_str());

            //check if the queue belongs to the input or output queues list
            bool is_input = (fnmatch("in:*", queuesData[i].name.c_str(),0) == 0);
            std::vector<QueueViewer*>* queuesList;
            QGroupBox* groupBoxTmp;
            QGridLayout* layoutTmp;
            if (is_input)
            {
                queuesList = &in_queues;
                groupBoxTmp = inputBox;
                layoutTmp = inputLayout;
            }
            else
            {
                queuesList = &out_queues;
                groupBoxTmp = outputBox;
                layoutTmp = outputLayout;

            }

            QueueViewer* queueTmp = find_queue(*queuesList, queuesData[i].name);
            bool current_queue_changed = false;
            if( queueTmp != NULL)
            {
                //Check if the data has actually changed
                current_queue_changed = !(queuesData[i] == queueTmp->data);
                if(current_queue_changed)
                {
                    any_change = true;
                    //queue already has a widget. Just update
                    queueTmp->setData(queuesData[i]);
                    //TODO: update data in queue tab
                }
            }
            else
            {
                //new queue. Create widget
                current_queue_changed = true;
                any_change = true;
                queueTmp = new QueueViewer(queuesData[i].name, groupBoxTmp);
                queueTmp->setData(queuesData[i]);
                queuesList->push_back(queueTmp);
                queueTmp->setLayout(layoutTmp, queuesList->size()-1);
                //layoutTmp->addWidget(queueTmp);
                
                groupBoxTmp->adjustSize();
               /* in_queues.push_back(queueTmp);
                inputLayout->addWidget(queueTmp);
                inputLayout->activate();*/
                //inputBox->update();
                //queueTmp->show();
                
            }
        }
        
        if(any_change)
        {
            //Update Totals
           QueueData totalData = totalQueues->data;
            
            totalData.kvs = au::str("%lu", totalKvs);
            totalData.size = au::str("%lu", totalSize);
            totalData.rate = au::str("%lu", totalRate);
            
            totalQueues->setData(totalData);
            totalQueues->update();
            
            //Hide or show emptiness labels
            if (in_queues.size() != 0)
            {
                noInputLabel->hide();
            }
            else
            {
                noInputLabel->show();
            }

            if (out_queues.size() != 0)
            {
                noOutputLabel->hide();
            }
            else
            {
                noOutputLabel->show();
            }

        }
            
        //ajustar layout o update el groupbox
     /*   inputBox->update();
        outputBox->update();
        totalBox->update();
        inputLayout->update();
        outputLayout->update();
        totalLayout->update();

        mainLayout->update();
        */
        
    }
    
    QueueViewer* DelilahMainWindow::find_queue(std::vector<QueueViewer*>& list, std::string name)
    {
        bool found = false;
        QueueViewer* queueTmp = NULL;
        for(unsigned int i = 0; i< list.size() && !found; i++)
        {
            if (list[i]->title == name)
            {
                found = true;
                queueTmp = list[i];
            }
        }
        
        return queueTmp;
        
    }
}
