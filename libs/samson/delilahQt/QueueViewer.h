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
    public:
        QueueViewer(QWidget* parent = 0, std::string title = "");
        typedef struct DataValue{
            QString name;
            QString value; 
        }

        str::vector<DataValue> data;
        
        void setData(str::vector<std::string> names, str::vector<std::string> values);
        void updateData(str::vector<std::string> values);
        void setTitle(std::string title);

    private:
        QGroupBox* groupBox;
        str::vector<QLabel*> values;
        QGridLayout* layout;
        
    
};

} //namespace

#endif //_H_Delilah_QT
