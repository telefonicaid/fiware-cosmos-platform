#ifndef _H_DelilahMainWindow
#define _H_DelilahMainWindow

/* ****************************************************************************
*
* FILE                     DelilahQT.h
*
* DESCRIPTION			   Main widget for delilahQt
*
*/


#include "logMsg/logMsg.h"				

#include <QtGui>

#include <vector>

#include "QueueViewer.h"
#include "ExtQueueViewer.h"

namespace samson {
	
	/**
	 Widget class for the DelilahQT GUI
	 */
    class DelilahMainWindow : public QMainWindow
    {
        Q_OBJECT
        
        QBoxLayout* mainLayout;
        /*QBoxLayout* queuesLayout;
        QBoxLayout* enginesLayout;*/

        QTabWidget* tabs;
        QWidget* queuesTab;
        QAction *aboutAction;
        QAction *exitAction;
        QMenu *fileMenu;
        
        QLabel* noInputLabel;
        QLabel* noOutputLabel;
        QLabel* noTotalLabel;
        QGridLayout* inputLayout;
        QGridLayout* outputLayout;
        QGridLayout* totalLayout;
        
        //std::vector<QueueViewer::QueueData> queuesData;
        //std::vector<std::string> current_queues;
        std::vector<QueueViewer*> in_queues;
        std::vector<QueueViewer*> out_queues;
        QueueViewer* totalQueues;
        
        std::vector<ExtQueueViewer*> tabbedQueues;
        
    public:
    
        DelilahMainWindow();
        //QueueViewer* queueViewer1;
        /*QLabel* enginesText;
        QLabel* queuesText;
        QGroupBox* queuesBox;
        QGroupBox* enginesBox;*/
        QGroupBox* inputBox;
        QGroupBox* outputBox;
        QGroupBox* totalBox;
        void updateData(std::vector<QueueData>& queuesData);
        
    private slots:
        void about();
        void onQueueDetailsClicked();

    signals:
        void requestUpdate();
        
    private:
        QueueViewer* findQueue(std::vector<QueueViewer*>& list, std::string name);
        ExtQueueViewer* findQueueTab(std::string name);
        QueueData getQueueData(std::string name, ExtQueueViewer* queue);
    };
}    
    
#endif    
