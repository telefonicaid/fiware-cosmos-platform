#ifndef _H_ExtQueueViewer
#define _H_ExtQueueViewer

/* ****************************************************************************
*
* FILE                     ExtQueueViewer.h
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/
#include <QtGui>
#include <vector>
#include <string>

namespace samson {
	

class  QueueData 
{
    public:
        std::string name;
        std::string kvs; //block_info/kv_info/kvs
        std::string  size; //block_info/size
        //Formats
        std::string key;
        std::string value;
        //Rate
        std::string total_kvs;
        std::string total_size;
        std::string kvs_s;
        std::string bytes_s;
        std::string blocks;
        std::string rate_size;
        std::string on_memory;
        std::string on_disk;
        std::string locked;
        std::string time_from;
        std::string time_to;

        bool operator==(const QueueData &other) const;
};

        
class ExtQueueViewer: public QWidget
{
        Q_OBJECT
    public:
        
        ExtQueueViewer(std::string _title, QWidget* parent = 0);
        std::string title;

        QueueData data;
        
        void setData(QueueData* newData);
        void setTitle(std::string title);
        void setHiddenButton(bool hidden);
        void updateFeed(std::string line);
        //void setLayout(QGridLayout* layout, int row);

        //we make this button public to be able to connect to it from DelilahMainWindow
        QPushButton* connectButton;
        QTextEdit* queueFeed;

    private:
        QLabel* name;
        QLabel* kvsLabel;
        QLabel* kvs;
        QLabel* sizeLabel;
        QLabel* size;
        QLabel* keyLabel;
        QLabel* key;
        QLabel* valueLabel;
        QLabel* value;
        QLabel* total_kvsLabel;
        QLabel* total_kvs;
        QLabel* total_sizeLabel;
        QLabel* total_size;
        QLabel* kvs_sLabel;
        QLabel* kvs_s;
        QLabel* bytes_sLabel;
        QLabel* bytes_s;
        QLabel* blocksLabel;
        QLabel* blocks;
        QLabel* rate_sizeLabel;
        QLabel* rate_size;
        QLabel* on_memoryLabel;
        QLabel* on_memory;
        QLabel* on_diskLabel;
        QLabel* on_disk;
        QLabel* lockedLabel;
        QLabel* locked;
        QLabel* time_fromLabel;
        QLabel* time_from;
        QLabel* time_toLabel;
        QLabel* time_to;
        
        //QPushButton* connectButton;
        //QTextEdit* queueFeed;
        
        QGroupBox* sizeBox;
        QGroupBox* formatBox;
        QGroupBox* rateBox;
        QGroupBox* blocksBox;
        
        QVBoxLayout* mainLayout;
        QHBoxLayout* connectButtonLayout;
        QHBoxLayout* generalLayout;
        QHBoxLayout* sizeLayout;
        QHBoxLayout* formatLayout;
        QHBoxLayout* rateLayout;
        QHBoxLayout* blocksLayout;
        
};

} //namespace

#endif //_H_Delilah_QT
