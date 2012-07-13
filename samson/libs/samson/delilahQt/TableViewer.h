#ifndef _H_TableViewer
#define _H_TableViewer

/* ****************************************************************************
*
* FILE                     TableViewer.h
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/
#include <QtGui>
#include <vector>
#include <string>

namespace samson {
	


        
class TableViewer: public QWidget
{
        Q_OBJECT
    public:
        
        TableViewer(QWidget* parent = 0, std::string title = "");

        typedef struct {
           QString name;
           QString value; 
        } DataValue;
        
        std::vector<DataValue> data;
        
        bool setData(std::vector<std::string> names, std::vector<std::string> values);
        void updateData(std::vector<std::string> values);
        void setTitle(std::string title);

    private:
        QGroupBox* groupBox;
        std::vector<QLabel*> nameLabels;
        std::vector<QLabel*> valueLabels;
        QVBoxLayout* layout;
        
    
};

} //namespace

#endif //_H_Delilah_QT
