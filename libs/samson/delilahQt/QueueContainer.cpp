/* ****************************************************************************
*
* FILE                     QueueContainer.cpp
*
* DESCRIPTION			   Widget to visualize samson workers' data
*
*/

#include "QueueContainer.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>
#include <fnmatch.h>

namespace samson{

QueueContainer::QueueContainer(QWidget* parent): QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    
    filterLayout = new QHBoxLayout;
    filterLabel =  new QLabel("Filter:");
    filterValue =  new QLineEdit(this);

    mainLayout->addLayout(filterLayout);
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterValue);
    filterLayout->addStretch();
    
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
    
    connect(filterValue, SIGNAL(editingFinished()), this, SLOT(onFilterEdited()));

}

void QueueContainer::setData(std::vector<QueueData*>& newQueuesData)
{

   std::cout << "QueueContainer::setData()- queues: " << queuesData.size() << std::endl;
        queuesData.clear();
        queuesData = newQueuesData;
        
        bool any_change = false;
        size_t totalKvs = 0;
        size_t totalSize = 0;
        size_t totalRate = 0;
        //For each queue, check if it has already a widget defined. otherwise create it
        for(unsigned int i = 0; i < queuesData.size(); i++)
        {
            //if the queue does not match the filter pattern, ignore it
            if(!filterValue->text().isEmpty() 
               && (fnmatch(filterValue->text().toAscii().constData(), queuesData[i]->name.c_str(),0) != 0))
            {
                continue;
            }
             
            //Increment totals
            totalKvs += atol(queuesData[i]->kvs.c_str());
            totalSize += atol(queuesData[i]->size.c_str());
            totalRate += atol(queuesData[i]->bytes_s.c_str());

            //check if the queue belongs to the input or output queues list
            bool is_input = (fnmatch("in:*", queuesData[i]->name.c_str(),0) == 0);
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
   std::cout << "Queue encontrada: " << queueTmp->title << std::endl;
            
                //Check if the data has actually changed
                current_queue_changed = !(*queuesData[i] == queueTmp->data);
                if(current_queue_changed)
                {
                    any_change = true;
                    //queue already has a widget. Just update
                    queueTmp->setData(queuesData[i]);
                    //update data in queue tab
                    emit queueHasChanged(queueTmp, queuesData[i]);
                    /*ExtQueueViewer* tabbedQueue = findQueueTab(queueTmp->title);
                    if (tabbedQueue)
                    {
                        tabbedQueue->setData(queuesData[i]);
                    }*/
                }
            }
            else
            {
   std::cout << "Queue creada: " << queuesData[i]->name << std::endl;
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
               /* in_queues.push_back(queueTmp);
                inputLayout->addWidget(queueTmp);
                inputLayout->activate();*/
                //inputBox->update();
                //queueTmp->show();
            }
            
            //Check if any queues have been deleted
            std::vector<QueueViewer*> deletedQueues = getDeletedQueues(queuesData); 
            if(deletedQueues.size() > 0) 
            {
                any_change = true;
                //Remove the widgets and delete the corresponding tabs if any
                for(std::vector<QueueViewer*>::iterator deletedIt = deletedQueues.begin(); deletedIt < deletedQueues.end(); deletedIt++)
                {
                    //Send signal to DelilahMainView to remove the tabs
                    emit(queueDeleted(*deletedIt));
                    //redo the widgets
                    QGridLayout* layoutTmp = (fnmatch("in:*", queuesData[i]->name.c_str(),0) == 0)?inputLayout:outputLayout;
                    layoutTmp->removeWidget(*deletedIt);
                    delete(*deletedIt);
                    inputBox->update();
                    outputBox->update();
                }
            }
        }
        
        if(any_change)
        {
            //Update Totals
            QueueData totalData = totalQueues->data;
            
            totalData.kvs = au::str("%lu", totalKvs);
            totalData.size = au::str("%lu", totalSize);
            totalData.bytes_s = au::str("%lu", totalRate);
            
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
            inputLayout->removeWidget(*it);
            delete(*it);
        }
        in_queues.clear();

        for(std::vector<QueueViewer*>::iterator it=out_queues.begin(); it<out_queues.end();it++)
        {
            outputLayout->removeWidget(*it);
            delete(*it);
        }
        out_queues.clear();
       
        //With everything empty, restart the queues
        setData(queuesData);
        
    }
    
    void QueueContainer::onFilterEdited()
    {
  std::cout <<   "onFilterEdited()" << std::endl;
        regenerateQueues();
    }
    
} //namespace
