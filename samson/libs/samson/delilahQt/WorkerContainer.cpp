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
* FILE                     WorkerContainer.cpp
*
* DESCRIPTION			   Widget to visualize samson workers' data
*
*/

#include "WorkerContainer.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>

namespace samson{

WorkerContainer::WorkerContainer(QWidget* parent): QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
}

void WorkerContainer::setData(std::vector<WorkerData*>& workersData)
{
    for (unsigned int i = 0; i<workersData.size(); i++)
    {
        WorkerViewer* workerTmp = findWorker(workersData[i]->worker_id);
        if(workerTmp)
        {
            //there is already a widget for this worker. Update data.
            workerTmp->setData(*workersData[i]);
            
        }
        else
        {
           //create new widget
            workerTmp =  new WorkerViewer(workersData[i]->worker_id, this);
            workerViewers.push_back(workerTmp);
            mainLayout->addWidget(workerTmp);
            //mainLayout->activate();
            update();
        }
    }
}

WorkerViewer* WorkerContainer::findWorker(std::string id)
{
    bool found = false;
    WorkerViewer* workerTmp = NULL;
    for(unsigned int i = 0; i< workerViewers.size() && !found; i++)
    {
        if (workerViewers[i]->worker_id == id)
        {
            found = true;
            workerTmp = workerViewers[i];
        }
    }
    return workerTmp;
}  
 

} //namespace
