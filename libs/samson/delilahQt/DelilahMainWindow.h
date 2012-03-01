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
#include "QueueContainer.h"
#include "WorkerContainer.h"

namespace samson {
    //A QTabWidget where you can decide which tabs have close buttons
    class ConfigurableTabWidget: public QTabWidget
    {
        public:
            ConfigurableTabWidget(QWidget * parent = 0 );
            void enableClosing(int index, bool enable);
    };
	
	/**
	 Widget class for the DelilahQT GUI
	 */
    class DelilahMainWindow : public QMainWindow
    {
        Q_OBJECT

        QBoxLayout* mainLayout;

        ConfigurableTabWidget* tabs;
        QueueContainer* queuesTab;
        WorkerContainer* workersTab;
        QAction *aboutAction;
        QAction *exitAction;
        QMenu *fileMenu;
        

        
        std::vector<ExtQueueViewer*> tabbedQueues;
        
    public:
    
        DelilahMainWindow();
        void updateData(std::vector<QueueData*> queuesData, std::vector<WorkerData*> workersData);
        void updateQueuesFeed(std::string data);
       
    public slots:
        void about();
        void onQueueDetailsClicked(QueueViewer* queue);
        void tabClosed(int index);
        void onQueueHasChanged(QueueViewer* queue, QueueData* data);
        void onQueueDeleted(QueueViewer* queue);
        void onConnectButtonClicked();

    signals:
        void requestUpdate();
        void connectedQueue(std::string name);
        void disconnectedQueue(std::string name);
        
    private:
        ExtQueueViewer* findQueueTab(std::string name);
        //QueueData getQueueData(std::string name, ExtQueueViewer* queue);
    };
}    
    
#endif    
