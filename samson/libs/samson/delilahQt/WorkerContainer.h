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
#ifndef _H_WorkerContainer
#define _H_WorkerContainer

/* ****************************************************************************
*
* FILE                     WorkerContainer.h
*
* DESCRIPTION			   Widget to visualize samson workers' data
*
*/

#include "WorkerViewer.h"

#include <QtGui>
#include <vector>
#include <string>

namespace samson {
        
class WorkerContainer: public QWidget
{
        Q_OBJECT
    public:
        
        WorkerContainer(QWidget* parent = 0);
        std::string worker_id;
        
        void setData(std::vector<WorkerData*>& workersData);

    private:
        
        std::vector<WorkerViewer*> workerViewers;
        QVBoxLayout* mainLayout;
        WorkerViewer* findWorker(std::string id);

};

} //namespace

#endif //_H_Delilah_QT
