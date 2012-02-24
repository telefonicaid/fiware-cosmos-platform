#ifndef _H_WorkerViewer
#define _H_WorkerViewer

/* ****************************************************************************
*
* FILE                     WorkerViewer.h
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/
#include <QtGui>
#include <vector>
#include <string>

namespace samson {
	

class  WorkerData 
{
    public:
        std::string worker_id;
        std::string type;
        std::string mem_used;
        std::string mem_total;
        std::string cores_used;
        std::string cores_total;
        std::string disk_ops;
        std::string disk_in_rate;
        std::string disk_out_rate;
        std::string net_in;
        std::string net_out;

        WorkerData();
        bool operator==(const WorkerData &other) const;
};

        
class WorkerViewer: public QWidget
{
        Q_OBJECT
    public:
        
        WorkerViewer(std::string _worker_id, QWidget* parent = 0);
        std::string worker_id;

        WorkerData data;
        
        void setData(WorkerData newData);
        void setTitle(std::string title);

    private:
    
        QGroupBox* groupBox;

        QGridLayout* mainLayout;
        QHBoxLayout* line1Layout;
        QHBoxLayout* line2Layout;
        QHBoxLayout* line3Layout;
        QHBoxLayout* line4Layout;

        QLabel* worker_idLabel;
        QLabel* worker_idValue;
        QLabel* typeLabel;
        QLabel* type;
        QLabel* memLabel;
        QLabel* mem;
        QProgressBar* memBar;
        QLabel* coresLabel;
        QLabel* cores;
        QProgressBar* coresBar;
        QLabel* disk_opsLabel;
        QLabel* disk_ops;
        QLabel* disk_in_rateLabel;
        QLabel* disk_in_rate;
        QLabel* disk_out_rateLabel;
        QLabel* disk_out_rate;
        QLabel* net_in_rateLabel;
        QLabel* net_in_rate;
        QLabel* net_out_rateLabel;
        QLabel* net_out_rate;
        
};

} //namespace

#endif //_H_Delilah_QT
