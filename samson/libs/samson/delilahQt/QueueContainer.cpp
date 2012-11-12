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
* FILE                     QueueContainer.cpp
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "QueueContainer.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>
#include <fnmatch.h>

namespace samson{

QueueContainer::QueueContainer(QWidget* parent): QScrollArea(parent)
{
    setAttribute( Qt::WA_DeleteOnClose );
    
    mainContainer = new QFrame(this);
    
    mainLayout = new QVBoxLayout(mainContainer);
    
    filterLayout = new QHBoxLayout;
    filterLabel =  new QLabel("Filter:");
    filterValue =  new QLineEdit(mainContainer);

    mainLayout->addLayout(filterLayout);
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterValue);
    filterLayout->addStretch();
    
    //Initialise groupboxes
    inputBox = new QGroupBox("Input Queues", mainContainer);
    outputBox = new QGroupBox("Output Queues", mainContainer);
    totalBox = new QGroupBox("Totals", mainContainer);

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
    totalInputQueues = new QueueViewer("Input", totalBox);
    totalInputQueues ->setHiddenButton(true);
    totalInputQueues->setLayout(totalLayout, 1);
    totalOutputQueues = new QueueViewer("Output", totalBox);
    totalOutputQueues ->setHiddenButton(true);
    totalOutputQueues->setLayout(totalLayout, 2);
    totalBox->setLayout(totalLayout);
    
    this->setWidget(mainContainer);
    this->setWidgetResizable(true);

    connect(filterValue, SIGNAL(editingFinished()), this, SLOT(onFilterEdited()));

}

void QueueContainer::setData(std::vector<QueueData*> newQueuesData)
{

        queuesData.clear();
        queuesData = newQueuesData;
        
        bool any_change = false;
        size_t totalKvs = 0;
        size_t totalSize = 0;
        size_t totalRate = 0;
        size_t totalRateKvs = 0;
        size_t totalInputKvs = 0;
        size_t totalInputSize = 0;
        size_t totalInputRate = 0;
        size_t totalInputRateKvs = 0;
        size_t totalOutputKvs = 0;
        size_t totalOutputSize = 0;
        size_t totalOutputRate = 0;
        size_t totalOutputRateKvs = 0;
        //For each queue, check if it has already a widget defined. otherwise create it
        for(unsigned int i = 0; i < queuesData.size(); i++)
        {
            //if the queue does not match the filter pattern, ignore it
            if(!filterValue->text().isEmpty() 
               && (fnmatch(filterValue->text().toAscii().constData(), queuesData[i]->name.c_str(),0) != 0))
            {
                continue;
            }
            
            //check if the queue belongs to the input or output queues list
            bool is_input = (fnmatch("in_*", queuesData[i]->name.c_str(),0) == 0);
            //Increment totals
            totalKvs += atol(queuesData[i]->kvs.c_str());
            totalSize += atol(queuesData[i]->size.c_str());
            totalRate += atol(queuesData[i]->bytes_s.c_str());
            totalRateKvs += atol(queuesData[i]->kvs_s.c_str());
            if(is_input)
            {
                totalInputKvs += atol(queuesData[i]->kvs.c_str());
                totalInputSize += atol(queuesData[i]->size.c_str());
                totalInputRate += atol(queuesData[i]->bytes_s.c_str());
                totalInputRateKvs += atol(queuesData[i]->kvs_s.c_str());
            }
            else
            {
                totalOutputKvs += atol(queuesData[i]->kvs.c_str());
                totalOutputSize += atol(queuesData[i]->size.c_str());
                totalOutputRate += atol(queuesData[i]->bytes_s.c_str());
                totalOutputRateKvs += atol(queuesData[i]->kvs_s.c_str());
            }

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

            QueueViewer* queueTmp = findQueue(*queuesList, queuesData[i]->name);
            
            bool current_queue_changed = false;
            if( queueTmp != NULL)
            {
            
                //Check if the data has actually changed
                current_queue_changed = !(*queuesData[i] == queueTmp->data);
                if(current_queue_changed)
                {
                    any_change = true;
                    //queue already has a widget. Just update
                    queueTmp->setData(queuesData[i]);
                    //update data in queue tab
                    emit queueHasChanged(queueTmp, queuesData[i]);
                }
            }
            else
            {
                //new queue. Create widget
                current_queue_changed = true;
                any_change = true;
                queueTmp = new QueueViewer(queuesData[i]->name, groupBoxTmp);
                queueTmp->setData(queuesData[i]);
                queuesList->push_back(queueTmp);
                queueTmp->setLayout(layoutTmp, queuesList->size()-1);
                connect(queueTmp, SIGNAL(detailsClicked()), this, SLOT(onQueueDetailsClicked()));
                //layoutTmp->addWidget(queueTmp);
                
                groupBoxTmp->adjustSize();
            }
            
        }
        
        
            //Check if any queues have been deleted
            std::vector<QueueViewer*> deletedQueues = getDeletedQueues(queuesData); 
            if(deletedQueues.size() > 0) 
            {
                any_change = true;
                regenerateQueues();
                for(unsigned int i = 0; i< deletedQueues.size();i++)
                {
                    emit(queueDeleted(deletedQueues[i]->title));
                }
            }

        if(any_change)
        {
            //Update Totals
            QueueData totalData = totalQueues->data;
            QueueData totalInputData = totalInputQueues->data;
            QueueData totalOutputData = totalOutputQueues->data;
            
            totalData.kvs = au::str("%lu", totalKvs);
            totalData.size = au::str("%lu", totalSize);
            totalData.bytes_s = au::str("%lu", totalRate);
            totalData.kvs_s = au::str("%lu", totalRateKvs);
            totalInputData.kvs = au::str("%lu", totalInputKvs);
            totalInputData.size = au::str("%lu", totalInputSize);
            totalInputData.bytes_s = au::str("%lu", totalInputRate);
            totalInputData.kvs_s = au::str("%lu", totalInputRateKvs);
            totalOutputData.kvs = au::str("%lu", totalOutputKvs);
            totalOutputData.size = au::str("%lu", totalOutputSize);
            totalOutputData.bytes_s = au::str("%lu", totalOutputRate);
            totalOutputData.kvs_s = au::str("%lu", totalOutputRateKvs);
            
            totalQueues->setData(totalData);
            totalQueues->update();
            totalInputQueues->setData(totalInputData);
            totalInputQueues->update();
            totalOutputQueues->setData(totalOutputData);
            totalOutputQueues->update();
            
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
}

    QueueViewer* QueueContainer::findQueue(std::vector<QueueViewer*>& list, std::string name_id)
    {
        bool found = false;
        QueueViewer* queueTmp = NULL;
        for(unsigned int i = 0; i< list.size() && !found; i++)
        {
            if (list[i]->title == name_id)
            {
                found = true;
                queueTmp = list[i];
            }
        }
        
        return queueTmp;
        
    }

    QueueData* QueueContainer::findQueue(std::vector<QueueData*>& list, std::string name_id)
    {
        bool found = false;
        QueueData* queueTmp = NULL;
        for(unsigned int i = 0; i< list.size() && !found; i++)
        {
            if (list[i]->name == name_id)
            {
                found = true;
                queueTmp = list[i];
            }
        }
        
        return queueTmp;
        
    }


    void QueueContainer::onQueueDetailsClicked()
    {
        //Get the queue button that triggered the slot
        QueueViewer* queue = (QueueViewer*) sender();
        emit queueDetailsClicked(queue);
    }

    std::vector<QueueViewer*> QueueContainer::getDeletedQueues(std::vector<QueueData*>& queuesData)
    {
        std::vector<QueueViewer*> deletedQueues;
        for (unsigned int i = 0; i< in_queues.size(); i++)
        {
            if (findQueue(queuesData, in_queues[i]->title) == NULL)
            {
                deletedQueues.push_back(in_queues[i]);
            }
        }

        for (unsigned int i = 0; i< out_queues.size(); i++)
        {
            if (findQueue(queuesData, out_queues[i]->title) == NULL)
            {
                deletedQueues.push_back(out_queues[i]);
            }
        }
        
        return deletedQueues;
        
    }
    
    void QueueContainer::regenerateQueues()
    {
         //First clean both in and out layouts
        for(std::vector<QueueViewer*>::iterator it=in_queues.begin(); it<in_queues.end();it++)
        {
            (*it)->hide();
            inputLayout->removeWidget(*it);
            delete(*it);
        }
        in_queues.clear();
        update();

        for(std::vector<QueueViewer*>::iterator it=out_queues.begin(); it<out_queues.end();it++)
        {
            (*it)->hide();
            outputLayout->removeWidget(*it);
            delete *it;
        }
        out_queues.clear();
        update();
       
        //With everything empty, restart the queues
        setData(queuesData);
        
    }
    
    void QueueContainer::onFilterEdited()
    {
        regenerateQueues();
    }
    
} //namespace
