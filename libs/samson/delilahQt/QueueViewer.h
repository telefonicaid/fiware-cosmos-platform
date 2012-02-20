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

namespace samson {
	


        
class QueueViewer: public QWidget
{
        Q_OBJECT
    public:
        
        QueueViewer(std::string _title, QWidget* parent = 0);
        std::string title;

        typedef struct {
            std::string name;
            std::string kvs; //block_info/kv_info/kvs
            //std::string kvsize; //block_info/kv_info/size
            std::string  size; //block_info/size
            //std::string  size_locked; //block_info/size_locked
            //std::string  size_on_disk; //block_info/size_on_disk
            //std::string  size_on_memory; //block_info/size_on_memory
                   
            //std::string kvs_rate; //rate_kvs/rate
            //std::string kvs_rate_size; //rate_kvs/size
            std::string rate; //rate_size/rate
            //std::string rate_size; //rate_size/size
        } QueueData;
        
        QueueData data;
        
        void setData(QueueData newData);
        void setTitle(std::string title);
        void setHiddenButton(bool hidden);

    private:
        //QScrollArea* scrollArea;
        //QGroupBox* groupBox;
        QHBoxLayout* layout;
        
        QLabel* name;
        QLabel* kvs;
        QLCDNumber* kvsDigits; 
        //QLabel* kvssize;
        QLabel* size;
        QLCDNumber* sizeDigits; 
        //QLabel* size_locked;
        //QLabel* size_on_disk;
        //QLabel* size_on_memory;
        //QLabel* kvs_rate;
        //QLabel* kvs_rate_size;
        QLabel* rate;
        QLCDNumber* rateDigits;
        QPushButton* detailsButton;
        //QLabel* rate_size;
        
};

} //namespace

#endif //_H_Delilah_QT
