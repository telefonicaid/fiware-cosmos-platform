#ifndef _H_QueueViewer
#define _H_QueueViewer

/* ****************************************************************************
*
* FILE                     QueueViewer.h
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/
#include <QtGui>
#include <vector>
#include <string>

#include "ExtQueueViewer.h"

namespace samson {
        
class QueueViewer: public QWidget
{
        Q_OBJECT
    public:
        
        QueueViewer(std::string _title, QWidget* parent = 0);
        std::string title;

        QueueData data;
        
        void setData(QueueData* newData);
        void setData(QueueData newData);
        void setTitle(std::string title);
        void setHiddenButton(bool hidden);
        void setLayout(QGridLayout* layout, int row);
        
        void destroyWidget();

        QPushButton* detailsButton;

    signals:
        void detailsClicked();

    public slots:
        void onDetailsClicked();

    private:
        //QScrollArea* scrollArea;
        //QGroupBox* groupBox;
        //QHBoxLayout* layout;
        
        QLabel* name;
        QLabel* kvs;
        QLabel* kvsDigits; 
        QLabel* kvs_s;
        QLabel* kvs_sDigits; 
        //QLabel* kvssize;
        QLabel* size;
        QLabel* sizeDigits; 
        //QLabel* size_locked;
        //QLabel* size_on_disk;
        //QLabel* size_on_memory;
        //QLabel* kvs_rate;
        //QLabel* kvs_rate_size;
        QLabel* rate;
        QLabel* rateDigits;
        //QLabel* rate_size;
        
};

} //namespace

#endif //_H_Delilah_QT
