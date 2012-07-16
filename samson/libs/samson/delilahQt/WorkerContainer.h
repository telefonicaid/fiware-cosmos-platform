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
