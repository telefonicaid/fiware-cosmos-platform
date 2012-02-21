/* ****************************************************************************
 *
 * FILE                     DelilahMainWindow.cpp
 *
 * DESCRIPTION			   Main widget for delilahQt
 *
 */

#include "DelilahMainWindow.h" // Own interface

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
        noTotalLabel = new QLabel("No Queues", totalBox);
        
        inputLayout = new QGridLayout(inputBox);
        inputLayout->addWidget(noInputLabel, 0, 1);
        inputBox->setLayout(inputLayout);
        
        outputLayout = new QGridLayout(outputBox);
        outputLayout->addWidget(noOutputLabel, 0, 1);
        outputBox->setLayout(outputLayout);

        totalLayout = new QGridLayout(totalBox);
        totalLayout->addWidget(noTotalLabel, 0 , 1);
        totalQueues = new QueueViewer("Totals", totalBox);
        totalLayout->addWidget(totalQueues);
        totalBox->setLayout(totalLayout);
                
        QObject::connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
        
        setMinimumSize(600,600); 
    }
    
    void DelilahMainWindow::about()
    {
        QMessageBox::about( this, tr("Samson"), tr("Samson version 0.6.1") );
    }
    
    void DelilahMainWindow::updateData(std::vector<QueueViewer::QueueData>& queuesData)
    {
        //For each queue, check if it has already a widget defined. otherwise create it
        for(unsigned int i = 0; i < queuesData.size(); i++)
        {
            //bool found = false;
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
            
           if( queueTmp != NULL)
            {
                //queue already has a widget. Just update
                queueTmp->setData(queuesData[i]);
            }
            else
            {
                //new queue. Create widget
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

        if (in_queues.size() != 0 || out_queues.size() != 0)
        {
            totalQueues->show();
            noTotalLabel->hide();
        }
        else
        {
            totalQueues->hide();
            noTotalLabel->show();
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
